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



/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        smbdef.h
 *
 * Abstract:
 *
 *        Likewise IO (LWIO)
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 *
 */
#ifndef __SMBDEF_H__
#define __SMBDEF_H__

#ifndef SMB_API
#define SMB_API
#endif

#define SMB_SECONDS_IN_MINUTE (60)
#define SMB_SECONDS_IN_HOUR   (60 * SMB_SECONDS_IN_MINUTE)
#define SMB_SECONDS_IN_DAY    (24 * SMB_SECONDS_IN_HOUR)

#ifndef SMB_MAX
#define SMB_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef SMB_MIN
#define SMB_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define SMB_DEFAULT_HANDLE_MAX 100000

#if defined(HAVE_SOCKLEN_T) && defined(GETSOCKNAME_TAKES_SOCKLEN_T)
#    define SOCKLEN_T socklen_t
#else
#    define SOCKLEN_T int
#endif

#define LW_ASSERT(x)   assert( (x) )

struct __LW_IO_ACCESS_TOKEN
{
    enum
    {
        IO_ACCESS_TOKEN_TYPE_PLAIN = 0,
        IO_ACCESS_TOKEN_TYPE_KRB5 = 1
    } type;
    union _LW_IO_ACCESS_TOKEN_U
    {
        struct _LW_IO_ACCESS_TOKEN_PLAIN
        {
            PWSTR pwszUsername;
            PWSTR pwszPassword;
        } plain;
        struct _LW_IO_ACCESS_TOKEN_KRB5
        {
            PWSTR pwszPrincipal;
            PWSTR pwszCachePath;
        } krb5;
    } payload;
};

typedef struct SMB_FILE_HANDLE SMB_FILE_HANDLE, *PSMB_FILE_HANDLE;

#endif /* __SMBDEF_H__ */
