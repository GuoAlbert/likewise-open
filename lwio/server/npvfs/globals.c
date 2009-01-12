/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software
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

#include "includes.h"

PSTR  gpszNPVFSProviderName = "Named Pipes Virtual File System";
NTVFS_DRIVER gNPVFSProviderTable =
{
        &NPVfsCreateFileEx,
        &NPVfsReadFileEx,
        &NPVfsWriteFileEx,
        &NPVfsGetSessionKey,
        &NPVfsCloseFileEx,
        &NPVfsTreeConnect,
        &NPVfsNTCreate,
        &NPVfsNTTransactCreate,
        &NPVfsCreateTemporary,
        &NPVfsReadFile,
        &NPVfsWriteFile,
        &NPVfsLockFile,
        &NPVfsSeekFile,
        &NPVfsFlushFile,
        &NPVfsCloseFile,
        &NPVfsCloseFileAndDisconnect,
        &NPVfsDeleteFile,
        &NPVfsRenameFile,
        &NPVfsCopyFile,
        &NPVfsTrans2QueryFileInformation,
        &NPVfsTrans2SetPathInformation,
        &NPVfsTrans2QueryPathInformation,
        &NPVfsTrans2CreateDirectory,
        &NPVfsTrans2DeleteDirectory,
        &NPVfsTrans2CheckDirectory,
        &NPVfsTrans2FindFirst2,
        &NPVfsTrans2FindNext2,
        &NPVfsNTTransactNotifyChange,
        &NPVfsTrans2GetDFSReferral
};
