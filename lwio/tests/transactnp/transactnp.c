/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software
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

#include "config.h"
#include "lsmbsys.h"

#include <lwio/lwio.h>

#include "smbdef.h"
#include "smbutils.h"

#include "smbwire.h"
#include "smbtransport.h"

#include "smbclient.h"

#define SMB_CONFIG_FILE_PATH CONFIGDIR "/lwiod.conf"

static
DWORD
GetHostAndShareNames(
    PCSTR pszShareName,
    PSTR* ppszShareName,
    PSTR* ppszHostname
    );

uint32_t
SMBSrvClientTreeOpen(
    PCSTR pszHostname,
    PCSTR pszPrincipal,
    PCSTR pszSharename,
    PSMB_TREE* ppTree
    );

uint32_t
NPOpen(
    SMB_TREE  *pTree,
    wchar16_t *pwszPath,
    uint16_t  *pFid
    );

uint32_t
NPTransact(
    SMB_TREE *pTree,
    uint16_t  fid,
    uint8_t  *pWrite,
    uint16_t writeLen,
    uint8_t  *pRead,
    uint16_t readLen
    );

uint32_t
NPClose(
    SMB_TREE *pTree,
    uint16_t  fid
    );

int
main(
    int argc,
    char* argv[]
    )
{
    DWORD dwError = 0;
    SMB_TREE *pTree = NULL;
    wchar16_t wszPipe[1024];
    uint16_t fid = 0;
    uchar8_t bar[1024];
    PSTR pszHostname = NULL;
    PSTR pszSharename = NULL;
    PCSTR pszPipename = NULL;
    BOOLEAN bCleanupSMBCore = FALSE;

    if (argc < 3)
    {
        printf("Usage: transactnp <share> <pipe name>\n\n");
        printf("Example: transactnp \\\\myhost.com\\IPC$ \\WKSSVC\n");
        exit(1);
    }

    dwError = GetHostAndShareNames(
                    argv[1],
                    &pszSharename,
                    &pszHostname);
    BAIL_ON_SMB_ERROR(dwError);

    pszPipename = argv[2];

    dwError = SMBSrvClientInit(SMB_CONFIG_FILE_PATH);
    BAIL_ON_SMB_ERROR(dwError);

    bCleanupSMBCore = TRUE;

    dwError = SMBSrvClientTreeOpen(
                    pszHostname,
                    "Administrator@KAYA-2K.CORP.CENTERIS.COM",
                    pszSharename,
                    &pTree);
    BAIL_ON_SMB_ERROR(dwError);

    if (!mbstowc16s(&wszPipe[0],
                    pszPipename,
                    sizeof(wszPipe)))
    {
        dwError = SMB_ERROR_INTERNAL;
        BAIL_ON_SMB_ERROR(dwError);
    }

    dwError = NPOpen(pTree, wszPipe, &fid);
    BAIL_ON_SMB_ERROR(dwError);

    dwError = NPTransact(
                    pTree,
                    fid,
                    (uint8_t *) "foo",
                    sizeof("foo"),
                    bar,
                    sizeof("bar"));
    BAIL_ON_SMB_ERROR(dwError);

cleanup:

    if (pTree)
    {
        NPClose(pTree, fid);
        SMBTreeRelease(pTree);
    }

    if (bCleanupSMBCore)
    {
        SMBSrvClientShutdown();
    }

    SMB_SAFE_FREE_STRING(pszHostname);
    SMB_SAFE_FREE_STRING(pszSharename);

    return dwError;

error:

    goto cleanup;
}

static
DWORD
GetHostAndShareNames(
    PCSTR pszShareName,
    PSTR* ppszShareName,
    PSTR* ppszHostname
    )
{
    DWORD dwError = 0;
    PCSTR pszInput = pszShareName;
    size_t sLength = 0;
    PSTR pszHostname = NULL;
    PSTR pszShareNameLocal = NULL;

    // Optional
    sLength = strspn(pszInput, "\\");
    if (sLength)
    {
        pszInput += sLength;
    }

    // Hostname
    sLength = strcspn(pszInput, "\\");
    if (!sLength)
    {
        dwError = SMB_ERROR_INVALID_PARAMETER;
        BAIL_ON_SMB_ERROR(dwError);
    }

    dwError = SMBStrndup(
                pszInput,
                (DWORD)sLength,
                &pszHostname);
    BAIL_ON_SMB_ERROR(dwError);

    pszInput += sLength;

    // Mandatory before share
    sLength = strspn(pszInput, "\\");
    if (!sLength)
    {
        dwError = SMB_ERROR_INVALID_PARAMETER;
        BAIL_ON_SMB_ERROR(dwError);
    }

    pszInput += sLength;

    // Share
    if (IsNullOrEmptyString(pszInput))
    {
        dwError = SMB_ERROR_INVALID_PARAMETER;
        BAIL_ON_SMB_ERROR(dwError);
    }

    dwError = SMBAllocateString(
                pszShareName,
                &pszShareNameLocal);
    BAIL_ON_SMB_ERROR(dwError);

    *ppszHostname = pszHostname;
    *ppszShareName = pszShareNameLocal;

cleanup:

    return dwError;

error:

    SMB_SAFE_FREE_STRING(pszHostname);
    SMB_SAFE_FREE_STRING(pszShareNameLocal);

    goto cleanup;
}

