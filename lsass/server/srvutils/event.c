/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        event.h
 *
 * Abstract:
 *
 *        Likewise Security And Authentication Subsystem (LSASS)
 *
 *        Eventlog API
 *
 * Authors: Wei Fu (wfu@likewisesoftware.com)
 */

#include "includes.h"

DWORD
LsaSrvOpenEventLog(
    PSTR pszCategoryType,
    PHANDLE phEventLog
    )
{
    return LWIOpenEventLogEx(
                  NULL,             // Server name (defaults to local computer eventlogd)
                  pszCategoryType,  // Table Category ID (Security, System, ...)
                  "Likewise LSASS", // Source
                  0,                // Source Event ID
                  "SYSTEM",         // User
                  NULL,             // Computer (defaults to assigning local hostname)
                  phEventLog);
}

DWORD
LsaSrvCloseEventLog(
    HANDLE hEventLog
    )
{
    return LWICloseEventLog(hEventLog);
}

DWORD
LsaSrvLogInformationEvent(
    HANDLE hEventLog,
    DWORD  dwEventID,
    PCSTR  pszUser, // NULL defaults to SYSTEM
    PCSTR  pszCategory,
    PCSTR  pszDescription,
    PCSTR  pszData
    )
{
    EVENT_LOG_RECORD event = {0};

    event.dwEventRecordId = 0;
    event.pszEventTableCategoryId = NULL;
    event.pszEventType = INFORMATION_EVENT_TYPE;
    event.dwEventDateTime = 0;
    event.pszEventSource = NULL;
    event.pszEventCategory = (PSTR) pszCategory;
    event.dwEventSourceId = dwEventID;
    event.pszUser = (PSTR) pszUser;
    event.pszComputer = NULL;
    event.pszDescription = (PSTR) pszDescription;
    event.pszData = (PSTR) pszData;

    return LWIWriteEventLogBase(
                   hEventLog,
                   event);
}

DWORD
LsaSrvLogWarningEvent(
    HANDLE hEventLog,
    DWORD  dwEventID,
    PCSTR  pszUser, // NULL defaults to SYSTEM
    PCSTR  pszCategory,
    PCSTR  pszDescription,
    PCSTR  pszData
    )
{
    EVENT_LOG_RECORD event = {0};

    event.dwEventRecordId = 0;
    event.pszEventTableCategoryId = NULL;
    event.pszEventType = WARNING_EVENT_TYPE;
    event.dwEventDateTime = 0;
    event.pszEventSource = NULL;
    event.pszEventCategory = (PSTR) pszCategory;
    event.dwEventSourceId = dwEventID;
    event.pszUser = (PSTR) pszUser;
    event.pszComputer = NULL;
    event.pszDescription = (PSTR) pszDescription;
    event.pszData = (PSTR) pszData;

    return LWIWriteEventLogBase(
                   hEventLog,
                   event);
}

DWORD
LsaSrvLogErrorEvent(
    HANDLE hEventLog,
    DWORD  dwEventID,
    PCSTR  pszUser, // NULL defaults to SYSTEM
    PCSTR  pszCategory,
    PCSTR  pszDescription,
    PCSTR  pszData
    )
{
    EVENT_LOG_RECORD event = {0};

    event.dwEventRecordId = 0;
    event.pszEventTableCategoryId = NULL;
    event.pszEventType = ERROR_EVENT_TYPE;
    event.dwEventDateTime = 0;
    event.pszEventSource = NULL;
    event.pszEventCategory = (PSTR) pszCategory;
    event.dwEventSourceId = dwEventID;
    event.pszUser = (PSTR) pszUser;
    event.pszComputer = NULL;
    event.pszDescription = (PSTR) pszDescription;
    event.pszData = (PSTR) pszData;

    return LWIWriteEventLogBase(
                   hEventLog,
                   event);
}

DWORD
LsaSrvLogSuccessAuditEvent(
    HANDLE hEventLog,
    DWORD  dwEventID,
    PCSTR  pszUser, // NULL defaults to SYSTEM
    PCSTR  pszCategory,
    PCSTR  pszDescription,
    PCSTR  pszData
    )
{
    EVENT_LOG_RECORD event = {0};

    event.dwEventRecordId = 0;
    event.pszEventTableCategoryId = NULL;
    event.pszEventType = SUCCESS_AUDIT_EVENT_TYPE;
    event.dwEventDateTime = 0;
    event.pszEventSource = NULL;
    event.pszEventCategory = (PSTR) pszCategory;
    event.dwEventSourceId = dwEventID;
    event.pszUser = (PSTR) pszUser;
    event.pszComputer = NULL;
    event.pszDescription = (PSTR) pszDescription;
    event.pszData = (PSTR) pszData;

    return LWIWriteEventLogBase(
                   hEventLog,
                   event);
}

DWORD
LsaSrvLogFailureAuditEvent(
    HANDLE hEventLog,
    DWORD  dwEventID,
    PCSTR  pszUser, // NULL defaults to SYSTEM
    PCSTR  pszCategory,
    PCSTR  pszDescription,
    PCSTR  pszData
    )
{
    EVENT_LOG_RECORD event = {0};

    event.dwEventRecordId = 0;
    event.pszEventTableCategoryId = NULL;
    event.pszEventType = FAILURE_AUDIT_EVENT_TYPE;
    event.dwEventDateTime = 0;
    event.pszEventSource = NULL;
    event.pszEventCategory = (PSTR) pszCategory;
    event.dwEventSourceId = dwEventID;
    event.pszUser = (PSTR) pszUser;
    event.pszComputer = NULL;
    event.pszDescription = (PSTR) pszDescription;
    event.pszData = (PSTR) pszData;

    return LWIWriteEventLogBase(
                   hEventLog,
                   event);
}

VOID
LsaSrvLogServiceSuccessEvent(
    DWORD dwEventID,
    PCSTR pszEventCategory,
    PCSTR pszDescription,
    PCSTR pszData
    )
{
    DWORD dwError = 0;
    HANDLE hEventLog = (HANDLE)NULL;
    
    dwError = LsaSrvOpenEventLog(
                  "System",
                  &hEventLog);       
    BAIL_ON_LSA_ERROR(dwError);    
    
    dwError = LsaSrvLogInformationEvent(            
                  hEventLog,
                  dwEventID,
                  NULL, // defaults to SYSTEM
                  pszEventCategory,
                  pszDescription,
                  pszData);
    BAIL_ON_LSA_ERROR(dwError);

cleanup:

    LsaSrvCloseEventLog(hEventLog);

    return;

error:

    LSA_LOG_ERROR("Failed to post service success event.");
    LSA_LOG_ERROR("Error code: [%d]", dwError);

    goto cleanup;
}

VOID
LsaSrvLogServiceWarningEvent(
    DWORD dwEventID,
    PCSTR pszEventCategory, 
    PCSTR pszDescription,
    PCSTR pszData
    )
{
    DWORD dwError = 0;
    HANDLE hEventLog = (HANDLE)NULL;
    
    dwError = LsaSrvOpenEventLog(
                  "System",
                  &hEventLog);       
    BAIL_ON_LSA_ERROR(dwError);    
    
    dwError = LsaSrvLogWarningEvent(            
                  hEventLog,
                  dwEventID,
                  NULL, // defaults to SYSTEM
                  pszEventCategory,
                  pszDescription,
                  pszData);
    BAIL_ON_LSA_ERROR(dwError);
    
cleanup:
    
    LsaSrvCloseEventLog(hEventLog);
    
    return;

error:

    LSA_LOG_ERROR("Failed to post service warning event.");
    LSA_LOG_ERROR("Error code: [%d]", dwError);

    goto cleanup;
}

VOID
LsaSrvLogServiceFailureEvent(
    DWORD dwEventID,
    PCSTR pszEventCategory,
    PCSTR pszDescription,
    PCSTR pszData
    )
{
    DWORD dwError = 0;
    HANDLE hEventLog = (HANDLE)NULL;
    
    dwError = LsaSrvOpenEventLog(
                  "System",
                  &hEventLog);       
    BAIL_ON_LSA_ERROR(dwError);    
    
    dwError = LsaSrvLogErrorEvent(            
                  hEventLog,
                  dwEventID,
                  NULL, // defaults to SYSTEM
                  pszEventCategory,
                  pszDescription,
                  pszData);
    BAIL_ON_LSA_ERROR(dwError);
    
cleanup:
    
    LsaSrvCloseEventLog(hEventLog);
    
    return;

error:

    LSA_LOG_ERROR("Failed to post service failure event.");
    LSA_LOG_ERROR("Error code: [%d]", dwError);

    goto cleanup;
}

VOID
LsaSrvLogUserIDConflictEvent(
    uid_t uid,
    PCSTR pszProviderName,
    DWORD dwErrCode
    )
{
    DWORD dwError = 0;
    PSTR pszUserIDConflictDescription = NULL;
    PSTR pszData = NULL;

    dwError = LsaAllocateStringPrintf(
                 &pszUserIDConflictDescription,
                 "Likewise account provisioning conflict.\r\n\r\n" \
                 "     Authentication provider: %s\r\n\r\n" \
                 "     Reason:                  Found duplicate entries for UIDs:\r\n" \
                 "     UID:                     %d",
                 pszProviderName,
                 uid);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaGetErrorMessageForLoggingEvent(
                         dwErrCode,
                         &pszData);

    LsaSrvLogServiceWarningEvent(
            LSASS_EVENT_WARNING_CONFIGURATION_ID_CONFLICT,
            SERVICE_EVENT_CATEGORY,
            pszUserIDConflictDescription,
            pszData);

cleanup:

    LSA_SAFE_FREE_STRING(pszUserIDConflictDescription);
    LSA_SAFE_FREE_STRING(pszData);

    return;

error:

    goto cleanup;

}

VOID
LsaSrvLogUserAliasConflictEvent(
    PCSTR pszAlias,
    PCSTR pszProviderName,
    DWORD dwErrCode
    )
{
    DWORD dwError = 0;
    PSTR pszUserAliasConflictDescription = NULL;
    PSTR pszData = NULL;

    dwError = LsaAllocateStringPrintf(
                 &pszUserAliasConflictDescription,
                 "Likewise account provisioning conflict.\r\n\r\n" \
                 "     Authentication provider: %s\r\n\r\n" \
                 "     Reason:                  Found duplicate entries for alias:\r\n" \
                 "     Alias:                   %s",
                 pszProviderName,
                 pszAlias);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaGetErrorMessageForLoggingEvent(
                         dwErrCode,
                         &pszData);

    LsaSrvLogServiceWarningEvent(
            LSASS_EVENT_WARNING_CONFIGURATION_ALIAS_CONFLICT,
            SERVICE_EVENT_CATEGORY,
            pszUserAliasConflictDescription,
            pszData);

cleanup:

    LSA_SAFE_FREE_STRING(pszUserAliasConflictDescription);
    LSA_SAFE_FREE_STRING(pszData);

    return;

error:

    goto cleanup;

}

VOID
LsaSrvLogDuplicateObjectFoundEvent(
    PCSTR pszName1,
    PCSTR pszName2,
    PCSTR pszProviderName,
    DWORD dwErrCode
    )
{
    DWORD dwError = 0;
    PSTR pszObjectDuplicateDescription = NULL;
    PSTR pszData = NULL;

    dwError = LsaAllocateStringPrintf(
                 &pszObjectDuplicateDescription,
                 "Likewise account provisioning conflict\r\n\r\n" \
                 "     Authentication provider: %s\r\n\r\n" \
                 "     Reason:                  Found duplicate entries for names:\r\n" \
                 "     Name 1:                  %s\r\n" \
                 "     Name 2:                  %s",
                 pszProviderName,
                 pszName1,
                 pszName2);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaGetErrorMessageForLoggingEvent(
                         dwErrCode,
                         &pszData);

    LsaSrvLogServiceWarningEvent(
            1020, // Lsass assigned object conflict event id
            SERVICE_EVENT_CATEGORY,
            pszObjectDuplicateDescription,
            pszData);

cleanup:

    LSA_SAFE_FREE_STRING(pszObjectDuplicateDescription);
    LSA_SAFE_FREE_STRING(pszData);

    return;

error:

    goto cleanup;

}

