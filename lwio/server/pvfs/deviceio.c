/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * Editor Settings: expandtabs and use 4 spaces for indentation */

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

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        deviceio.c
 *
 * Abstract:
 *
 *        Likewise Posix File System Driver (PVFS)
 *
 *        Device I/O Control handler
 *
 * Authors: Gerald Carter <gcarter@likewise.com>
 */

#include "pvfs.h"

struct _PVFS_DEVICECTL_DISPATCH_TABLE
{
    ULONG DeviceCtlCode;
    NTSTATUS (*fn)(
        IN     PPVFS_IRP_CONTEXT pIrpContext,
        IN     PVOID  InputBuffer,
        IN     ULONG  InputBufferLength,
        OUT    PVOID  OutputBuffer,
        IN OUT PULONG pOutputBufferLength);

} PvfsDeviceCtlHandlerTable[] = {
    { IO_DEVICE_CTL_OPEN_FILE_INFO,       PvfsIoCtlOpenFileInfo }
};



NTSTATUS
PvfsDispatchDeviceIoControl(
    PPVFS_IRP_CONTEXT  pIrpContext
    )
{
    NTSTATUS ntError = STATUS_UNSUCCESSFUL;
    PIRP pIrp = pIrpContext->pIrp;
    IRP_ARGS_IO_FS_CONTROL Args = pIrp->Args.IoFsControl;
    ULONG DeviceCtlCode = Args.ControlCode;
    ULONG i = 0;
    ULONG TableSize = sizeof(PvfsDeviceCtlHandlerTable) /
                      sizeof(struct _PVFS_DEVICECTL_DISPATCH_TABLE);
    PPVFS_CCB pCcb = NULL;

    ntError =  PvfsAcquireCCB(pIrpContext->pIrp->FileHandle, &pCcb);
    BAIL_ON_NT_STATUS(ntError);

#if 0
    if (!IsSetFlag(pCcb->Flags, PVFS_CCB_FLAG_CREATE_COMPLETE))
    {
        ntError = STATUS_INVALID_PARAMETER;
        BAIL_ON_NT_STATUS(ntError);
    }
#endif

    /* Loop through the dispatch table.  Levels included in the table
       but having a NULL handler get NOT_IMPLEMENTED while those not in
       the table at all get NOT_SUPPORTED. */

    for (i=0; i<TableSize; i++)
    {
        if (PvfsDeviceCtlHandlerTable[i].DeviceCtlCode == DeviceCtlCode)
        {
            if (PvfsDeviceCtlHandlerTable[i].fn == NULL)
            {
                ntError = STATUS_NOT_IMPLEMENTED;
                break;
            }

            ntError = PvfsDeviceCtlHandlerTable[i].fn(
                          pIrpContext,
                          Args.InputBuffer,
                          Args.InputBufferLength,
                          Args.OutputBuffer,
                          &Args.OutputBufferLength);
            break;
        }
    }

    if (i == TableSize) {
        ntError = STATUS_NOT_SUPPORTED;
    }
    BAIL_ON_NT_STATUS(ntError);

    pIrp->IoStatusBlock.BytesTransferred = Args.OutputBufferLength;

cleanup:

    if (pCcb)
    {
        PvfsReleaseCCB(pCcb);
    }

    return ntError;

error:
    goto cleanup;
}

