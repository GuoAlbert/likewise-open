/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

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
 *        evtstack.h
 *
 * Abstract:
 *
 *        Likewise Server Service
 *
 *        Stack
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *
 */

#include "includes.h"

DWORD
SrvSvcStackPush(
    PVOID pItem,
    PSRVSVC_STACK* ppStack
    )
{
    DWORD dwError = 0;
    PSRVSVC_STACK pStack = NULL;

    if (!ppStack) {
        dwError = SRVSVC_ERROR_INVALID_PARAMETER;
        BAIL_ON_SRVSVC_ERROR(dwError);
    }

    dwError = LwAllocateMemory(
                    sizeof(SRVSVC_STACK),
                    (PVOID*)&pStack);
    BAIL_ON_SRVSVC_ERROR(dwError);

    pStack->pItem = pItem;

    pStack->pNext = *ppStack;
    *ppStack = pStack;

cleanup:
    return dwError;

error:
    LW_SAFE_FREE_MEMORY(pStack);

    goto cleanup;
}

PVOID
SrvSvcStackPop(
    PSRVSVC_STACK* ppStack
    )
{
    PVOID pItem = NULL;
    PSRVSVC_STACK pTop = (ppStack && *ppStack ? *ppStack : NULL);

    if (pTop)
    {
        *ppStack = pTop->pNext;
        pItem    = pTop->pItem;

        LW_SAFE_FREE_MEMORY(pTop);
    }

    return pItem;
}

PVOID
SrvSvcStackPeek(
    PSRVSVC_STACK pStack
    )
{
    return (pStack ? pStack->pItem : NULL);
}

DWORD
SrvSvcStackForeach(
    PSRVSVC_STACK pStack,
    PFN_SRVSVC_FOREACH_STACK_ITEM pfnAction,
    PVOID pUserData
    )
{
    DWORD dwError = 0;
    PSRVSVC_STACK pIter = pStack;

    if (!pfnAction) {
        goto cleanup;
    }

    for (; pIter; pIter = pIter->pNext)
    {
        dwError = pfnAction(pIter->pItem, pUserData);
        BAIL_ON_SRVSVC_ERROR(dwError);
    }

cleanup:
    return dwError;

error:
    goto cleanup;
}

PSRVSVC_STACK
SrvSvcStackReverse(
    PSRVSVC_STACK pStack
    )
{
    PSRVSVC_STACK pP = NULL;
    PSRVSVC_STACK pQ = pStack;
    PSRVSVC_STACK pR = NULL;

    while ( pQ ) {
        pR = pQ->pNext;
        pQ->pNext = pP;
        pP = pQ;
        pQ = pR;
    }

    return pP;
}

VOID
SrvSvcStackFree(
    PSRVSVC_STACK pStack
    )
{
    while (pStack)
    {
        PSRVSVC_STACK pTmp = pStack;

        pStack = pStack->pNext;

        LW_SAFE_FREE_MEMORY(pTmp);
    }
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
