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
 *        smbloginfo.h
 *
 * Abstract:
 *
 *        Likewise IO (LWIO)
 * 
 *        Log Info
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */

#include "includes.h"

DWORD
SMBBuildLogInfo(
    SMBLogLevel    maxAllowedLogLevel,
    SMBLogTarget   logTarget,
    PCSTR          pszPath,
    PSMB_LOG_INFO* ppLogInfo
    )
{
    DWORD dwError = 0;
    PSMB_LOG_INFO pLogInfo = NULL;
    
    BAIL_ON_INVALID_POINTER(ppLogInfo);
    
    dwError = SMBAllocateMemory(
                    sizeof(SMB_LOG_INFO),
                    (PVOID*)&pLogInfo);
    BAIL_ON_SMB_ERROR(dwError);
    
    if (!IsNullOrEmptyString(pszPath))
    {
        dwError = SMBAllocateString(
                        pszPath,
                        &pLogInfo->pszPath);
        BAIL_ON_SMB_ERROR(dwError);
    }
    
    pLogInfo->maxAllowedLogLevel  = maxAllowedLogLevel;
    pLogInfo->logTarget = logTarget;
    
    *ppLogInfo = pLogInfo;
    
cleanup:

    return dwError;
    
error:
    
    if (ppLogInfo)
    {
        *ppLogInfo = NULL;
    }
    
    if (pLogInfo)
    {
        SMBFreeLogInfo(pLogInfo);
    }
    
    goto cleanup;
}

VOID
SMBFreeLogInfo(
    PSMB_LOG_INFO pLogInfo
    )
{
    SMB_SAFE_FREE_STRING(pLogInfo->pszPath);
    SMBFreeMemory(pLogInfo);
}
