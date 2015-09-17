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

#ifndef __DJ_API_H__
#define __DJ_API_H__

#include <lwexc.h>

typedef struct
{
    BOOLEAN noModifyHosts;
    char* logFile;
    
/* Callbacks */
    void (*warn)(
	const char* shortMsg, 
	const char* longMsg, 
	LWException** exc);
    void (*progress)(
	double ratio,
	const char* operationMsg,
	LWException** exc);
} DJOptions;

DWORD
DJInit(
    VOID
    );

DWORD
DJJoinDomain(
    PCSTR pszDomain,
    PCSTR pszOU,
    PCSTR pszUsername,
    PCSTR pszPassword
    );

DWORD
DJQueryJoinInformation(
    PSTR* ppszComputerName,
    PSTR* ppszDomainName,
    PSTR* ppszComputerDN
    );

DWORD
DJUnjoinDomain(
    PCSTR pszUsername,
    PCSTR pszPassword
    );

DWORD
DJConfigureNSSwitch(
    VOID
    );

DWORD
DJUnconfigureNSSwitch(
    VOID
    );

DWORD
DJConfigurePAM(
    VOID
    );

DWORD
DJUnconfigurePAM(
    VOID
    );

DWORD
DJConfigureSSH(
    VOID
    );

DWORD
DJUnconfigureSSH(
    VOID
    );

VOID
DJFreeMemory(
    PVOID pMemory
    );

DWORD
DJShutdown(
    VOID
    );

void
DJQuery(
    char **computer, 
    char **domain,
    DJOptions* options,
    LWException** exc
    );

void
DJRenameComputer(
    const char* computer,
    const char* domain,
    DJOptions* options,
    LWException** exc
    );

#endif
