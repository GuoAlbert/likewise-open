/*
 * Copyright (C) Centeris Corporation 2004-2007
 * Copyright (C) Likewise Software    2007-2008
 * All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation; either version 2.1 of 
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>.
 */

/* ex: set tabstop=4 expandtab shiftwidth=4: */
#include "domainjoin.h"
#include "djdistroinfo.h"
#include "djpamconf.h"
#include "djaixparser.h"

#define GCE(x) GOTO_CLEANUP_ON_CENTERROR((x))

static PCSTR LOGIN_CONFIG_PATH = "/etc/security/login.cfg";

static
CENTERROR
GetAuthType(const DynamicArray *lines, PSTR *result)
{
    CENTERROR ceError = DJGetOptionValue(lines, "usw", "auth_type", result);
    if(ceError == CENTERROR_CFG_VALUE_NOT_FOUND)
    {
        //return the default
        ceError = CTStrdup("STD_AUTH", result);
    }
    return ceError;
}

static
CENTERROR
SetAuthType(DynamicArray *lines, PCSTR value)
{
    return DJSetOptionValue(lines, "usw", "auth_type", value);
}

CENTERROR
DJFixLoginConfigFile(
    PCSTR pszPath
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    PCSTR pszFilePath = NULL;
    PSTR pszTmpPath = NULL;
    BOOLEAN bFileExists = FALSE;
    FILE* fp = NULL;
    FILE* fp_new = NULL;
    DynamicArray lines;
    PSTR currentSystem = NULL;

    memset(&lines, 0, sizeof(lines));
    if (IsNullOrEmptyString(pszPath))
        pszFilePath = LOGIN_CONFIG_PATH;
    else
        pszFilePath = pszPath;

    GCE(ceError = CTCheckFileExists(pszFilePath, &bFileExists));

    if (!bFileExists)
        goto cleanup;

    GCE(ceError = CTOpenFile(pszFilePath, "r", &fp));
    GCE(ceError = CTReadLines(fp, &lines));
    GCE(ceError = CTSafeCloseFile(&fp));

    GCE(ceError = GetAuthType(&lines, &currentSystem));
    if(!strcmp(currentSystem, "PAM_AUTH"))
        goto cleanup;

    GCE(ceError = SetAuthType(&lines, "PAM_AUTH"));

    GCE(ceError = CTAllocateStringPrintf(&pszTmpPath, "%s.new", pszFilePath));

    GCE(ceError = CTOpenFile(pszTmpPath, "w", &fp_new));
    GCE(ceError = CTWriteLines(fp_new, &lines));
    GCE(ceError = CTSafeCloseFile(&fp_new));
    GCE(ceError = CTCloneFilePerms(pszFilePath, pszTmpPath));
    GCE(ceError = CTBackupFile(pszFilePath));
    GCE(ceError = CTMoveFile(pszTmpPath, pszFilePath));

cleanup:
    CTSafeCloseFile(&fp);
    CTSafeCloseFile(&fp_new);

    CT_SAFE_FREE_STRING(currentSystem);
    CT_SAFE_FREE_STRING(pszTmpPath);
    CTFreeLines(&lines);

    return ceError;
}

static QueryResult QueryPamMode(const JoinProcessOptions *options, LWException **exc)
{
    BOOLEAN bFileExists = FALSE;
    DistroInfo distro;
    QueryResult result = NotApplicable;
    FILE* fp = NULL;
    DynamicArray lines;
    PCSTR pszFilePath = LOGIN_CONFIG_PATH;
    PSTR currentSystem = NULL;

    memset(&lines, 0, sizeof(lines));
    memset(&distro, 0, sizeof(distro));

    if(!options->joiningDomain)
        goto cleanup;

    LW_CLEANUP_CTERR(exc, DJGetDistroInfo(NULL, &distro));
    if(distro.os != OS_AIX || strcmp(distro.version, "5.3"))
        goto cleanup;

    LW_CLEANUP_CTERR(exc, CTCheckFileExists(pszFilePath, &bFileExists));

    if (!bFileExists)
        goto cleanup;

    result = SufficientlyConfigured;

    LW_CLEANUP_CTERR(exc, CTOpenFile(pszFilePath, "r", &fp));
    LW_CLEANUP_CTERR(exc, CTReadLines(fp, &lines));
    LW_CLEANUP_CTERR(exc, CTSafeCloseFile(&fp));

    LW_CLEANUP_CTERR(exc, GetAuthType(&lines, &currentSystem));
    if(strcmp(currentSystem, "PAM_AUTH"))
        goto cleanup;

    result = FullyConfigured;

cleanup:
    CT_SAFE_FREE_STRING(currentSystem);
    DJFreeDistroInfo(&distro);
    CTSafeCloseFile(&fp);
    CTFreeLines(&lines);
    return result;
}

static void DoPamMode(JoinProcessOptions *options, LWException **exc)
{
    if(options->joiningDomain)
    {
        LW_CLEANUP_CTERR(exc, DJAddMissingAIXServices(NULL));
        LW_CLEANUP_CTERR(exc, DJFixLoginConfigFile(NULL));
    }
cleanup:
    ;
}

static PSTR GetPamModeDescription(const JoinProcessOptions *options, LWException **exc)
{
    PSTR ret = NULL;
    LW_CLEANUP_CTERR(exc, CTAllocateStringPrintf( &ret,
"By default AIX will use LAM to perform authentication requests, but AIX 5.3 may instead use PAM for authentication. PAM allows for richer text in prompts and error messages, so it is recommended that the system be switched into PAM mode. This will be done by setting 'auth_type = PAM_AUTH' in the 'usw' stanza in /etc/security/login.cfg.\n"
"\n"
"A few of the programs that ship with AIX are not enabled in the default pam.conf. Before switching the system into PAM mode, entries are made in pam.conf for these services:"
"\tsshd\n"
"\tsudo\n"
"\tdtsession"));

cleanup:
    return ret;
}

const JoinModule DJPamMode = { TRUE, "pam-mode", "switch authentication from LAM to PAM", QueryPamMode, DoPamMode, GetPamModeDescription };
