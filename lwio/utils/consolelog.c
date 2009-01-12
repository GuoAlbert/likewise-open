/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        consolelog.c
 *
 * Abstract:
 *
 *        Likewise IO (LWIO)
 * 
 *        Logging API
 * 
 *        Implemenation of logging to file
 *
 * Authors: Sriram Nambakam (snambakam@likewisesoftware.com)
 * 
 */

#include "includes.h"

DWORD
SMBOpenConsoleLog(
    SMBLogLevel maxAllowedLogLevel,
    PHANDLE     phLog
    )
{
    DWORD dwError = 0;
    PSMB_CONSOLE_LOG pConsoleLog = NULL;
    
    dwError = SMBAllocateMemory(
                    sizeof(SMB_CONSOLE_LOG),
                    (PVOID*)&pConsoleLog);
    if (dwError)
    {
        goto error;
    }
    
    pConsoleLog->fp_out = stdout;
    pConsoleLog->fp_err = stderr;
    
    dwError = SMBSetupLogging(
                    (HANDLE)pConsoleLog,
                    maxAllowedLogLevel,
                    &SMBLogToConsole
                    );
    if (dwError)
    {
        goto error;
    }
    
    *phLog = (HANDLE)pConsoleLog;

cleanup:

    return dwError;

error:

    *phLog = (HANDLE)NULL;
    
    if (pConsoleLog)
    {
        SMBFreeConsoleLogInfo(pConsoleLog);
    }

    goto cleanup;
}

DWORD
SMBCloseConsoleLog(
    HANDLE hLog
    )
{
    PSMB_CONSOLE_LOG pConsoleLog = (PSMB_CONSOLE_LOG)hLog;
    
    SMBResetLogging();
    
    if (pConsoleLog)
    {
        SMBFreeConsoleLogInfo(pConsoleLog);
    }
    return 0;
}

VOID
SMBLogToConsole(
    HANDLE      hLog,
    SMBLogLevel logLevel,
    PCSTR       pszFormat,
    va_list     msgList
    )
{
    PSMB_CONSOLE_LOG pConsoleLog = (PSMB_CONSOLE_LOG)hLog;
    PSTR pszEntryType = NULL;
    time_t currentTime;
    struct tm tmp = {0};
    char timeBuf[128];
    FILE* pTarget = NULL;
    
    switch (logLevel)
    {
        case SMB_LOG_LEVEL_ALWAYS:
        {
            pszEntryType = SMB_INFO_TAG;
            pTarget = pConsoleLog->fp_out;
            break;
        }
        case SMB_LOG_LEVEL_ERROR:
        {
            pszEntryType = SMB_ERROR_TAG;
            pTarget = pConsoleLog->fp_err;
            break;
        }

        case SMB_LOG_LEVEL_WARNING:
        {
            pszEntryType = SMB_WARN_TAG;
            pTarget = pConsoleLog->fp_err;
            break;
        }

        case SMB_LOG_LEVEL_INFO:
        {
            pszEntryType = SMB_INFO_TAG;
            pTarget = pConsoleLog->fp_out;
            break;
        }

        case SMB_LOG_LEVEL_VERBOSE:
        {
            pszEntryType = SMB_VERBOSE_TAG;
            pTarget = pConsoleLog->fp_out;
            break;
        }

        case SMB_LOG_LEVEL_DEBUG:
        {
            pszEntryType = SMB_DEBUG_TAG;
            pTarget = pConsoleLog->fp_out;
            break;
        }

        default:
        {
            pszEntryType = SMB_VERBOSE_TAG;
            pTarget = pConsoleLog->fp_out;
            break;
        }
    }

    currentTime = time(NULL);
    localtime_r(&currentTime, &tmp);

    strftime(timeBuf, sizeof(timeBuf), SMB_LOG_TIME_FORMAT, &tmp);

    fprintf(pTarget, "%s:%s:", timeBuf, pszEntryType);
    vfprintf(pTarget, pszFormat, msgList);
    fprintf(pTarget, "\n");
    fflush(pTarget);
}

VOID
SMBFreeConsoleLogInfo(
    PSMB_CONSOLE_LOG pConsoleLog
    )
{
    SMBFreeMemory(pConsoleLog);
}
