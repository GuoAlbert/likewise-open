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

static const PSTR ERROR_TAG   = "ERROR";
static const PSTR WARN_TAG    = "WARNING";
static const PSTR INFO_TAG    = "INFO";
static const PSTR VERBOSE_TAG = "VERBOSE";

static const PSTR LOG_TIME_FORMAT = "%Y%m%d%H%M%S";

LOGINFO gdjLogInfo =
{
    LOG_LEVEL_ERROR,
    LOG_DISABLED,
    {"", NULL}
};

CENTERROR
dj_validate_log_level(
    DWORD dwLogLevel
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;

    if (dwLogLevel < 1) {
        ceError = CENTERROR_INVALID_VALUE;
    }

    return (ceError);
}

CENTERROR
dj_disable_logging()
{
    CENTERROR ceError = CENTERROR_SUCCESS;

    if (gdjLogInfo.logfile.logHandle != NULL &&
        gdjLogInfo.logfile.logHandle != stdout) {
        fclose(gdjLogInfo.logfile.logHandle);
    }

    gdjLogInfo.logfile.logHandle = NULL;
    gdjLogInfo.dwLogTarget = LOG_DISABLED;

    return ceError;
}

CENTERROR
dj_init_logging_to_console(
    DWORD dwLogLevel
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;

    if (gdjLogInfo.logfile.logHandle != NULL &&
        gdjLogInfo.logfile.logHandle != stdout) {
        fclose(gdjLogInfo.logfile.logHandle);
    }

    gdjLogInfo.logfile.logHandle = stdout;
    gdjLogInfo.dwLogTarget = LOG_TO_FILE;

    ceError = dj_set_log_level(dwLogLevel);
    BAIL_ON_CENTERIS_ERROR(ceError);

error:
    return ceError;
}

CENTERROR
dj_init_logging_to_file(
    DWORD dwLogLevel,
    PSTR  pszLogFilePath
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;

    gdjLogInfo.dwLogTarget = LOG_TO_FILE;

    strncpy(gdjLogInfo.logfile.szLogPath, pszLogFilePath, PATH_MAX);
    *(gdjLogInfo.logfile.szLogPath+PATH_MAX+1) = '\0';

    gdjLogInfo.logfile.logHandle = NULL;
    if (gdjLogInfo.logfile.szLogPath[0] != '\0') {
        ceError = CTOpenFile(gdjLogInfo.logfile.szLogPath, "a",
            &gdjLogInfo.logfile.logHandle);
        BAIL_ON_CENTERIS_ERROR(ceError);
        ceError = CTChangePermissions(gdjLogInfo.logfile.szLogPath, 0600);
        BAIL_ON_CENTERIS_ERROR(ceError);
        ceError = CTSetCloseOnExec(gdjLogInfo.logfile.logHandle);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }

    ceError = dj_set_log_level(dwLogLevel);
    BAIL_ON_CENTERIS_ERROR(ceError);

    return (ceError);

error:

    if (gdjLogInfo.logfile.logHandle != NULL) {
        fclose(gdjLogInfo.logfile.logHandle);
        gdjLogInfo.logfile.logHandle = NULL;
    }

    return (ceError);
}

CENTERROR
dj_init_logging_to_file_handle(
    DWORD dwLogLevel,
    FILE* handle
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;

    gdjLogInfo.dwLogTarget = LOG_TO_FILE;


    gdjLogInfo.logfile.logHandle = handle;

    ceError = dj_set_log_level(dwLogLevel);
    BAIL_ON_CENTERIS_ERROR(ceError);

    return (ceError);

error:

    if (gdjLogInfo.logfile.logHandle != NULL) {
        fclose(gdjLogInfo.logfile.logHandle);
        gdjLogInfo.logfile.logHandle = NULL;
    }

    return (ceError);
}

void
dj_close_log()
{
    if (gdjLogInfo.logfile.logHandle != NULL &&
        gdjLogInfo.logfile.logHandle != stdout) {
        fclose(gdjLogInfo.logfile.logHandle);
        gdjLogInfo.logfile.logHandle = NULL;
    }
}

void
dj_log_to_file(
    DWORD dwLogLevel,
    PSTR pszFormat,
    va_list msgList
    )
{
    PSTR pszEntryType = NULL;
    time_t currentTime;
    struct tm tmp;
    char timeBuf[256];

    switch (dwLogLevel)
    {
    case LOG_LEVEL_ALWAYS:
    {
        pszEntryType = INFO_TAG;
        break;
    }
    case LOG_LEVEL_ERROR:
    {
        pszEntryType = ERROR_TAG;
        break;
    }

    case LOG_LEVEL_WARNING:
    {
        pszEntryType = WARN_TAG;
        break;
    }

    case LOG_LEVEL_INFO:
    {
        pszEntryType = INFO_TAG;
        break;
    }

    default:
    {
        pszEntryType = VERBOSE_TAG;
        break;
    }
    }

    currentTime = time(NULL);
    localtime_r(&currentTime, &tmp);

    strftime(timeBuf, sizeof(timeBuf), LOG_TIME_FORMAT, &tmp);

    fprintf(gdjLogInfo.logfile.logHandle, "%s:%s:", timeBuf, pszEntryType);
    vfprintf(gdjLogInfo.logfile.logHandle, pszFormat, msgList);
    fprintf(gdjLogInfo.logfile.logHandle, "\n");
    fflush(gdjLogInfo.logfile.logHandle);
}

void
dj_log_message(
    DWORD dwLogLevel,
    PSTR pszFormat,...
    )
{
    va_list argp;

    if ( gdjLogInfo.dwLogTarget == LOG_DISABLED ||
         gdjLogInfo.logfile.logHandle == NULL ) {
        return;
    }

    if (gdjLogInfo.dwLogLevel < dwLogLevel) {
        return;
    }

    va_start(argp, pszFormat);

    dj_log_to_file(dwLogLevel, pszFormat, argp);

    va_end(argp);
}

CENTERROR
dj_set_log_level(
    DWORD dwLogLevel
    )
{
    DWORD ceError = CENTERROR_SUCCESS;

    ceError = dj_validate_log_level(dwLogLevel);
    BAIL_ON_CENTERIS_ERROR(ceError);

    gdjLogInfo.dwLogLevel = dwLogLevel;

error:

    return (ceError);
}

CENTERROR DJLogException(
    DWORD dwLogLevel,
    const LWException *exc)
{
    PSTR exceptionString = NULL;
    CENTERROR ceError = CENTERROR_SUCCESS;

    ceError = LWExceptionToString(exc, NULL, TRUE, TRUE, &exceptionString);
    BAIL_ON_CENTERIS_ERROR(ceError);

    dj_log_message(dwLogLevel, exceptionString);

error:
    CT_SAFE_FREE_STRING(exceptionString);
    return ceError;
}
