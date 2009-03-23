/*
 * Copyright (c) Likewise Software.  All rights Reserved.
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
 * license@likewise.com
 */

/*
 * Module Name:
 *
 *        errno.c
 *
 * Abstract:
 *
 *        UNIX errno codes
 *
 * Authors: Brian Koropoff (bkoropoff@likewise.com)
 *
 */

#include "includes.h"

typedef struct _TABLE_ENTRY
{
    int code;
    NTSTATUS ntStatus;
    PCSTR pszSymbolicName;
} const TABLE_ENTRY, *PTABLE_ENTRY;

static
PTABLE_ENTRY
LwUnixErrnoLookupCode(
    IN int code
    );

#define ERRNO_CODE(code, nt) { code, nt, #code },
static
TABLE_ENTRY LwErrnoCodeTable[] =
{
#include "errno-table.h"
    {-1, -1, NULL}
};
#undef ERRNO_CODE

PCSTR
LwUnixErrnoToSymbolicName(
    IN int code
    )
{
    PTABLE_ENTRY pEntry = LwUnixErrnoLookupCode(code);

    if (pEntry)
    {
        return pEntry->pszSymbolicName;
    }
    else
    {
        return "UNKNOWN";
    }
}

NTSTATUS
LwUnixErrnoToNtStatus(
    IN int code
    )
{
    PTABLE_ENTRY pEntry = LwUnixErrnoLookupCode(code);

    if (pEntry)
    {
        return pEntry->ntStatus;
    }
    else
    {
        return -1;
    }
}

static
PTABLE_ENTRY
LwUnixErrnoLookupCode(
    IN int code
    )
{
    ULONG index;

    for (index = 0; index < sizeof(LwErrnoCodeTable) / sizeof(*LwErrnoCodeTable); index++)
    {
        if (LwErrnoCodeTable[index].code == code)
        {
            return &LwErrnoCodeTable[index];
        }
    }

    return NULL;
}

