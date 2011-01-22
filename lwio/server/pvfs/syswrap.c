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
 *        syswrap.c
 *
 * Abstract:
 *
 *        Likewise Posix File System Driver (PVFS)
 *
 *        Syscall wrapper functions
 *
 * Authors: Gerald Carter <gcarter@likewise.com>
 */

#include "pvfs.h"

static
NTSTATUS
PvfsSysGetDiskFileName(
    OUT PSTR* ppszDiskFilename,
    IN PPVFS_FILE_NAME pFileName
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    PSTR pszDiskFilename = NULL;
    PSTR pszDirname = NULL;
    PSTR pszBasename = NULL;
    PSTR pszStreamParentDirname = NULL;
    PSTR pszStreamDirname = NULL;

    PVFS_BAIL_ON_INVALID_FILENAME(pFileName, ntError);

    if (PvfsIsDefaultStreamName(pFileName))
    {
        ntError = LwRtlCStringDuplicate(&pszDiskFilename,
                                        pFileName->FileName);
        BAIL_ON_NT_STATUS(ntError);
    }
    else
    {
        ntError = PvfsFileDirname(&pszDirname,
                                  pFileName->FileName);
        BAIL_ON_NT_STATUS(ntError);

        ntError = LwRtlCStringAllocatePrintf(
                      &pszStreamParentDirname,
                      "%s/%s",
                      pszDirname,
                      PVFS_STREAM_METADATA_DIR_NAME);
        BAIL_ON_NT_STATUS(ntError);

        ntError = PvfsSysOpenDir(pszStreamParentDirname, NULL);
        if (LW_STATUS_OBJECT_NAME_NOT_FOUND == ntError)
        {
            // create meta data directory
            ntError = PvfsSysMkDir(
                          pszStreamParentDirname,
                          (mode_t)gPvfsDriverConfig.CreateDirectoryMode);
            BAIL_ON_NT_STATUS(ntError);
        }
        BAIL_ON_NT_STATUS(ntError);

        ntError = PvfsFileBasename(&pszBasename,
                                  pFileName->FileName);
        BAIL_ON_NT_STATUS(ntError);

        ntError = LwRtlCStringAllocatePrintf(
                      &pszStreamDirname,
                      "%s/%s",
                      pszStreamParentDirname,
                      pszBasename);
        BAIL_ON_NT_STATUS(ntError);

        ntError = PvfsSysOpenDir(pszStreamDirname, NULL);
        if (LW_STATUS_OBJECT_NAME_NOT_FOUND == ntError)
        {
            // create stream directory for an object
            ntError = PvfsSysMkDir(
                          pszStreamDirname,
                          (mode_t)gPvfsDriverConfig.CreateDirectoryMode);
            BAIL_ON_NT_STATUS(ntError);
        }
        BAIL_ON_NT_STATUS(ntError);

        ntError = LwRtlCStringAllocatePrintf(
                      &pszDiskFilename,
                      "%s/%s",
                      pszStreamDirname,
                      pFileName->StreamName);
        BAIL_ON_NT_STATUS(ntError);
    }

    *ppszDiskFilename = pszDiskFilename;

cleanup:
    if (pszDirname)
    {
        LwRtlCStringFree(&pszDirname);
    }

    if (pszBasename)
    {
        LwRtlCStringFree(&pszBasename);
    }

    if (pszStreamParentDirname)
    {
        LwRtlCStringFree(&pszStreamParentDirname);
    }

    if (pszStreamDirname)
    {
        LwRtlCStringFree(&pszStreamDirname);
    }

    return ntError;

error:
    if (pszDiskFilename)
    {
        LwRtlCStringFree(&pszDiskFilename);
    }

    goto cleanup;
}

/**********************************************************
 *********************************************************/

static
NTSTATUS
CopyUnixStatToPvfsStat(
    PPVFS_STAT pPvfsStat,
    struct stat *pSbuf
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;

    pPvfsStat->s_mode    = pSbuf->st_mode;
    pPvfsStat->s_ino     = pSbuf->st_ino;
    pPvfsStat->s_dev     = pSbuf->st_dev;
    pPvfsStat->s_rdev    = pSbuf->st_rdev;
    pPvfsStat->s_nlink   = pSbuf->st_nlink;
    pPvfsStat->s_uid     = pSbuf->st_uid;
    pPvfsStat->s_gid     = pSbuf->st_gid;
    pPvfsStat->s_size    = pSbuf->st_size;
    pPvfsStat->s_alloc   = pSbuf->st_blocks * 512;
    pPvfsStat->s_atime   = pSbuf->st_atime;
    pPvfsStat->s_ctime   = pSbuf->st_ctime;
    pPvfsStat->s_mtime   = pSbuf->st_mtime;
    pPvfsStat->s_crtime  = pSbuf->st_mtime;
    pPvfsStat->s_blksize = pSbuf->st_blksize;
    pPvfsStat->s_blocks  = pSbuf->st_blocks;

    return ntError;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysStat(
	PCSTR pszFilename,
	PPVFS_STAT pStat
	)
{
    NTSTATUS ntError = STATUS_SUCCESS;
    struct stat sBuf = {0};
    int unixerr = 0;

    if (stat(pszFilename, &sBuf) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    if (pStat) {
        ntError = CopyUnixStatToPvfsStat(pStat, &sBuf);
        BAIL_ON_NT_STATUS(ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

////////////////////////////////////////////////////////////////////////

NTSTATUS
PvfsSysStatByFileName(
    IN PPVFS_FILE_NAME FileName,
	IN OUT PPVFS_STAT Stat
	)
{
    NTSTATUS ntError = STATUS_SUCCESS;
    struct stat sBuf = {0};
    int unixerr = 0;
    PSTR fileName = NULL;

    ntError = PvfsSysGetDiskFileName(&fileName, FileName);
    BAIL_ON_NT_STATUS(ntError);

    if (stat(fileName, &sBuf) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    if (Stat) {
        ntError = CopyUnixStatToPvfsStat(Stat, &sBuf);
        BAIL_ON_NT_STATUS(ntError);
    }

cleanup:
    if (fileName)
    {
        LwRtlCStringFree(&fileName);
    }

    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysFstat(
    int fd,
	PPVFS_STAT pStat
	)
{
    NTSTATUS ntError = STATUS_SUCCESS;
    struct stat sBuf = {0};
    int unixerr = 0;

    if (fstat(fd, &sBuf) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    if (pStat) {
        ntError = CopyUnixStatToPvfsStat(pStat, &sBuf);
        BAIL_ON_NT_STATUS(ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}


/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysOpen(
    int *pFd,
    PSTR pszFilename,
    int iFlags,
    mode_t Mode
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int fd = -1;
    int unixerr = 0;

    BAIL_ON_INVALID_PTR(pszFilename, ntError);

    if ((fd = open(pszFilename, iFlags, Mode)) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    *pFd = fd;

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysOpenByFileName(
    OUT int *pFd,
    IN PPVFS_FILE_NAME pFileName,
    IN int iFlags,
    IN mode_t Mode
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int fd = -1;
    int unixerr = 0;
    PSTR pszFilename = NULL;

    ntError = PvfsSysGetDiskFileName(&pszFilename, pFileName);
    BAIL_ON_NT_STATUS(ntError);

    if ((fd = open(pszFilename, iFlags, Mode)) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    *pFd = fd;

cleanup:
    if (pszFilename)
    {
        LwRtlCStringFree(&pszFilename);
    }

    return ntError;

error:
    goto cleanup;
}


/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysMkDir(
    PSTR pszDirname,
    mode_t mode
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if ((mkdir(pszDirname, mode)) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}


////////////////////////////////////////////////////////////////////////

NTSTATUS
PvfsSysMkDirByFileName(
    IN PPVFS_FILE_NAME DirectoryName,
    mode_t mode
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;
    PSTR directoryName = NULL;

    ntError = PvfsAllocateCStringFromFileName(&directoryName, DirectoryName);
    BAIL_ON_NT_STATUS(ntError);

    if ((mkdir(directoryName, mode)) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

error:
    if (directoryName)
    {
        LwRtlCStringFree(&directoryName);
    }

    return ntError;
}


/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysOpenDir(
    IN PCSTR pszDirname,
    OUT OPTIONAL DIR **ppDir
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;
    DIR *pDir = NULL;

    if ((pDir = opendir(pszDirname)) == NULL) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

error:
    if (ntError || !ppDir)
    {
        if (pDir)
        {
            PvfsSysCloseDir(pDir);
        }
    }

    if (ppDir)
    {
        *ppDir = pDir;
    }

    return ntError;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysDirFd(
    PPVFS_CCB pCcb,
    int *pFd
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;
    int fd = -1;

#ifdef HAVE_DIRFD
    if ((fd = dirfd(pCcb->pDirContext->pDir)) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }
#else
    if ((fd = open(pCcb->pszFilename, 0, 0)) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }
#endif

    *pFd = fd;

cleanup:
    return ntError;

error:
    goto cleanup;
}



/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysReadDir(
    DIR *pDir,
    struct dirent *pDirEntry,
    struct dirent **ppDirEntry
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    unixerr = readdir_r(pDir, pDirEntry, ppDirEntry);
    ntError = PvfsMapUnixErrnoToNtStatus(unixerr);
    BAIL_ON_NT_STATUS(ntError);

error:

    return ntError;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysCloseDir(
    DIR *pDir
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (closedir(pDir) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysClose(
    int fd
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (close(fd) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}


/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysLseek(
    IN int fd,
    IN off_t offset,
    IN int whence,
    OUT OPTIONAL off_t *pNewOffset
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;
    off_t newOffset = 0;

    if ((newOffset = lseek(fd, offset, whence)) == (off_t)-1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    if (pNewOffset) {
        *pNewOffset = newOffset;
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysFtruncate(
    int fd,
    off_t offset
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (ftruncate(fd, offset) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysUtime(
    PSTR pszPathname,
    LONG64 LastWriteTime,
    LONG64 LastAccessTime
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr;
    time_t tWrite = 0;
    time_t tAccess = 0;
    struct utimbuf TimeBuf = {0};

    PVFS_ZERO_MEMORY(&TimeBuf);

    if (LastWriteTime != 0) {
        ntError = PvfsWinToUnixTime(&tWrite, LastWriteTime);
        BAIL_ON_NT_STATUS(ntError);
    }

    if (LastAccessTime != 0) {
        ntError = PvfsWinToUnixTime(&tAccess, LastAccessTime);
        BAIL_ON_NT_STATUS(ntError);
    }

    TimeBuf.actime = tAccess;
    TimeBuf.modtime = tWrite;

    if (utime(pszPathname, &TimeBuf) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysFstatFs(
    PPVFS_CCB pCcb,
    PPVFS_STATFS pStatFs
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;

#if defined(HAVE_FSTATFS) && defined(__LWI_LINUX__)
    {
        struct statfs sFsBuf;
        int unixerr = 0;

        if (fstatfs(pCcb->fd, &sFsBuf) == -1)
        {
            PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
        }

        pStatFs->BlockSize         =
                sFsBuf.f_bsize > UINT32_MAX ? UINT32_MAX : sFsBuf.f_bsize;
        pStatFs->TotalBlocks       = sFsBuf.f_blocks;
        pStatFs->TotalFreeBlocks   = sFsBuf.f_bavail;
        pStatFs->MaximumNameLength =
                sFsBuf.f_namelen > INT32_MAX ? INT32_MAX : sFsBuf.f_namelen;

        ntError = STATUS_SUCCESS;
    }
#else
    /* Make up some numbers */
    pStatFs->BlockSize         = 4096;
    pStatFs->TotalBlocks       = 1024*128;
    pStatFs->TotalFreeBlocks   = 1024*64;
    pStatFs->MaximumNameLength = 255;

    ntError = STATUS_SUCCESS;
    BAIL_ON_NT_STATUS(ntError);
#endif

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysRemove(
    PCSTR pszPath
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (remove(pszPath) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

////////////////////////////////////////////////////////////////////////

NTSTATUS
PvfsSysRemoveByFileName(
    IN PPVFS_FILE_NAME FileName
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;
    PSTR fileName = NULL;

    ntError = PvfsSysGetDiskFileName(&fileName, FileName);
    BAIL_ON_NT_STATUS(ntError);

    if (remove(fileName) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

error:
    if (fileName)
    {
        LwRtlCStringFree(&fileName);
    }

    return ntError;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysRead(
    PPVFS_CCB pCcb,
    PVOID pBuffer,
    ULONG pBufLen,
    PULONG64 pOffset,
    PULONG pBytesRead
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    ssize_t bytesRead = 0;
    int unixerr = 0;

    /* Use pread() if we have an offset, otherwise fall back
       to read() */

    if (pOffset)
    {
        off_t offset = 0;

        bytesRead = pread(pCcb->fd, pBuffer, pBufLen, (off_t)*pOffset);

        if (bytesRead > 0)
        {
            /* pread() and pwrite() don't update the file offset */
            ntError = PvfsSysLseek(pCcb->fd, bytesRead, SEEK_CUR, &offset);
            BAIL_ON_NT_STATUS(ntError);
        }
    }
    else
    {
        bytesRead = read(pCcb->fd, pBuffer, pBufLen);
    }

    if (bytesRead == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    *pBytesRead = (ULONG)bytesRead;

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysWrite(
    PPVFS_CCB pCcb,
    PVOID pBuffer,
    ULONG pBufLen,
    PULONG64 pOffset,
    PULONG pBytesWritten
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    ssize_t bytesWritten = 0;
    int unixerr = 0;

    /* Use pwrite() if we have an offset, otherwise fall back
       to write() */

    if (pOffset)
    {
        off_t offset = 0;

        bytesWritten = pwrite(pCcb->fd, pBuffer, pBufLen, (off_t)*pOffset);

        if (bytesWritten > 0)
        {
            /* pread() and pwrite() don't update the file offset */
            ntError = PvfsSysLseek(pCcb->fd, bytesWritten, SEEK_CUR, &offset);
            BAIL_ON_NT_STATUS(ntError);
        }
    }
    else
    {
         bytesWritten = write(pCcb->fd, pBuffer, pBufLen);
    }

    if (bytesWritten == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

    *pBytesWritten = (ULONG)bytesWritten;

cleanup:
    return ntError;

error:
    goto cleanup;
}

////////////////////////////////////////////////////////////////////////

NTSTATUS
PvfsSysChownByFileName(
    IN PPVFS_FILE_NAME pFileName,
    uid_t uid,
    gid_t gid
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;
    PSTR pszFilename = NULL;

    ntError = PvfsSysGetDiskFileName(&pszFilename, pFileName);
    BAIL_ON_NT_STATUS(ntError);

    if (chown(pszFilename, uid, gid) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

error:
    if (pszFilename)
    {
        LwRtlCStringFree(&pszFilename);
    }

    return ntError;
}


/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysFchmod(
    PPVFS_CCB pCcb,
    mode_t Mode
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (fchmod(pCcb->fd, Mode) == -1) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysRename(
    PCSTR pszOldname,
    PCSTR pszNewname
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (rename(pszOldname, pszNewname) == -1 ) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

////////////////////////////////////////////////////////////////////////

NTSTATUS
PvfsSysRenameByFileName(
    IN PPVFS_FILE_NAME OriginalFileName,
    IN PPVFS_FILE_NAME NewFileName
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;
    PSTR oldPath = NULL;
    PSTR newPath = NULL;

    ntError = PvfsSysGetDiskFileName(&oldPath, OriginalFileName);
    BAIL_ON_NT_STATUS(ntError);

    ntError = PvfsSysGetDiskFileName(&newPath, NewFileName);
    BAIL_ON_NT_STATUS(ntError);

    if (rename(oldPath, newPath) == -1 )
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

error:
    if (oldPath)
    {
        LwRtlCStringFree(&oldPath);
    }

    if (newPath)
    {
        LwRtlCStringFree(&newPath);
    }

    return ntError;
}

/**********************************************************
 *********************************************************/

#define PVFS_DISABLE_FSYNC   1

NTSTATUS
PvfsSysFsync(
    PPVFS_CCB pCcb
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

#ifdef PVFS_DISABLE_FSYNC
    ntError = STATUS_SUCCESS;
    unixerr = 0;
    BAIL_ON_NT_STATUS(ntError);
#elif defined(HAVE_FDATASYNC)
    if (fdatasync(pCcb->fd) == -1 ) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }
#elif defined(HAVE_FSYNC)
    if (fsync(pCcb->fd) == -1 ) {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }
#else
    ntError = STATUS_NOT_SUPPORTED;
    BAIL_ON_NT_STATUS(ntError);
#endif

cleanup:
    return ntError;

error:
    goto cleanup;
}


/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysNanoSleep(
    const struct timespec *pRequestedTime,
    struct timespec *pRemainingTime
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (nanosleep(pRequestedTime, pRemainingTime) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysPipe(
    int PipeFds[2]
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (pipe(PipeFds) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    PipeFds[0] = -1;
    PipeFds[1] = -1;

    goto cleanup;
}

/**********************************************************
 *********************************************************/

NTSTATUS
PvfsSysSetNonBlocking(
    int Fd
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    int unixerr = 0;

    if (fcntl(Fd, F_SETFL, O_NONBLOCK) == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    return ntError;

error:
    goto cleanup;
}

/**********************************************************
 *********************************************************/

#ifdef HAVE_SPLICE
NTSTATUS
PvfsSysSplice(
    int FromFd,
    PLONG64 pFromOffset,
    int ToFd,
    PLONG64 pToOffset,
    ULONG Length,
    unsigned int Flags,
    PULONG pBytesSpliced
    )
{
    NTSTATUS ntError = STATUS_SUCCESS;
    long result = 0;
    int unixerr = 0;

    result = splice(FromFd, pFromOffset, ToFd, pToOffset, Length, Flags);
    if (result == -1)
    {
        PVFS_BAIL_ON_UNIX_ERROR(unixerr, ntError);
    }

cleanup:
    *pBytesSpliced = result;

    return ntError;

error:
    result = 0;
    goto cleanup;
}
#endif

////////////////////////////////////////////////////////////////////////

NTSTATUS
PvfsSysEnumStreams(
    IN PPVFS_CCB pCcb,
    OUT PPVFS_FILE_NAME *ppStreamNames,
    OUT PLONG StreamCount
    )
{
    NTSTATUS ntError =  STATUS_SUCCESS;
    LONG streamNameListLength = 0;
    PPVFS_FILE_NAME streamNameList = NULL;
    PPVFS_FILE_NAME fileName = NULL;
    PPVFS_FILE_NAME parentDirectoryName = NULL;
    PPVFS_FILE_NAME baseFileName = NULL;
    PSTR streamDirectory = NULL;
    PPVFS_DIRECTORY_CONTEXT streamDirectoryContext = NULL;
    DIR *pDir = NULL;
    struct dirent *pDirEntry = NULL;
    struct dirent dirEntry = { 0 };
    LONG i = 0;
    LONG currentIndex = 0;

    if (!PvfsIsDefaultStream(pCcb->pScb))
    {
        // Enumeration is only valid on the base file object itself
        ntError = STATUS_INVALID_PARAMETER;
        BAIL_ON_NT_STATUS(ntError);
    }

    ntError = PvfsAllocateFileNameFromScb(&fileName, pCcb->pScb);
    BAIL_ON_NT_STATUS(ntError);

    ntError = PvfsSplitFileNamePath(
                  &parentDirectoryName,
                  &baseFileName,
                  fileName);
    BAIL_ON_NT_STATUS(ntError);

    ntError = LwRtlCStringAllocatePrintf(
                  &streamDirectory,
                  "%s/%s/%s",
                  PvfsGetCStringBaseFileName(parentDirectoryName),
                  PVFS_STREAM_METADATA_DIR_NAME,
                  PvfsGetCStringBaseFileName(baseFileName));
    BAIL_ON_NT_STATUS(ntError);

    ntError = PvfsAllocateMemory(
                  (PVOID*)&streamDirectoryContext,
                  sizeof(*streamDirectoryContext),
                  TRUE);
    BAIL_ON_NT_STATUS(ntError);

    ntError = PvfsSysOpenDir(streamDirectory, &streamDirectoryContext->pDir);
    if (ntError == STATUS_SUCCESS)
    {
        pDir = streamDirectoryContext->pDir;

        for (ntError = PvfsSysReadDir(pDir, &dirEntry, &pDirEntry);
             pDirEntry;
             ntError = PvfsSysReadDir(pDir, &dirEntry, &pDirEntry))
        {
            BAIL_ON_NT_STATUS(ntError);

            if (RtlCStringIsEqual(pDirEntry->d_name, ".", FALSE) ||
                RtlCStringIsEqual(pDirEntry->d_name, "..", FALSE))
            {
                continue;
            }

            ntError = PvfsDirContextAddEntry(streamDirectoryContext, pDirEntry->d_name);
            BAIL_ON_NT_STATUS(ntError);
        }
    }

    // Always include space
    ntError = PvfsAllocateFileNameList(
                  &streamNameList,
                  streamDirectoryContext->dwNumEntries+1);
    BAIL_ON_NT_STATUS(ntError);

    ntError = PvfsAppendBuildFileName(
                  &streamNameList[currentIndex],
                  parentDirectoryName,
                  baseFileName);
    BAIL_ON_NT_STATUS(ntError);

    ntError = PvfsCopyStreamFileNameFromCString(
                  &streamNameList[currentIndex],
                  "");
    BAIL_ON_NT_STATUS(ntError);

    currentIndex++;

    for (i=0; i<streamDirectoryContext->dwNumEntries; i++)
    {
        ntError = PvfsAppendBuildFileName(
                      &streamNameList[currentIndex],
                      parentDirectoryName,
                      baseFileName);
        BAIL_ON_NT_STATUS(ntError);

        ntError = PvfsCopyStreamFileNameFromCString(
                      &streamNameList[currentIndex],
                      streamDirectoryContext->pDirEntries[i].pszFilename);
        BAIL_ON_NT_STATUS(ntError);

        currentIndex++;
    }

    *ppStreamNames = streamNameList;
    *StreamCount = currentIndex;

error:
    if (!NT_SUCCESS(ntError))
    {
        if (streamNameList)
        {
            PvfsFreeFileNameList(streamNameList, streamNameListLength);
        }
    }

    if (pDir)
    {
        PvfsSysCloseDir(pDir);
        streamDirectoryContext->pDir = NULL;
    }

    if (parentDirectoryName)
    {
        PvfsFreeFileName(parentDirectoryName);
    }
    if (baseFileName)
    {
        PvfsFreeFileName(baseFileName);
    }

    PvfsFreeDirectoryContext(streamDirectoryContext);

    return ntError;
}

