/*
 * Copyright (C) Centeris Corporation 2004-2007
 * Copyright (C) Likewise Software    2007-2008
 * All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation; either version 2.1 of 
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>.
 */

/* ex: set tabstop=4 expandtab shiftwidth=4: */
#include "ctexec.h"
#include <sys/wait.h>

CENTERROR
CTCaptureOutput(
    PCSTR command,
    PSTR* output
    )
{
    return CTCaptureOutputWithStderr(command, FALSE, output);
}

CENTERROR
CTCaptureOutputWithStderr(
    PCSTR command,
    BOOLEAN captureStderr,
    PSTR* output
    )
{
    PCSTR args[] = { "/bin/sh", "-c", command, (char*) NULL };
    return CTCaptureOutputWithStderrEx( args[0], args, captureStderr, output, NULL);
}

CENTERROR
CTCaptureOutputWithStderrEx(
    PCSTR command,
    PCSTR* ppszArgs,
    BOOLEAN captureStderr,
    PSTR* output,
    int *exitCode
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    unsigned int buffer_size = 1024;
    unsigned int read_size, write_size;
    int out[2];
    int pid, status;
    PSTR tempOutput = NULL;

    if(output != NULL)
        *output = NULL;
    
    if (pipe(out))
    {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);   
    }
    
    pid = fork();
    
    if (pid < 0)
    {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);     
    }
    else if (pid == 0)
    {
        // Child process
        if (dup2(out[1], STDOUT_FILENO) < 0)
            abort();
        if (captureStderr && dup2(out[1], STDERR_FILENO) < 0)
            abort();
        if (close(out[0]))
            abort();
        if (close(out[1]))
            abort();
        execvp(command, (char **)ppszArgs);
    }
    
    if (close(out[1]))
    {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);     
    }
    
    ceError = CTAllocateMemory(buffer_size, (PVOID*) &tempOutput);
    BAIL_ON_CENTERIS_ERROR(ceError);
    
    write_size = 0;
    
    while ((read_size = read(out[0], tempOutput + write_size, buffer_size - write_size)) > 0)
    {
        write_size += read_size;
        if (write_size == buffer_size)
        {
            buffer_size *= 2;
            ceError = CTReallocMemory(tempOutput, (PVOID*) &tempOutput, buffer_size);
            BAIL_ON_CENTERIS_ERROR(ceError);
        }
    }
    
    if (read_size < 0)
    {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError); 
    }
    
    if (close(out[0]))
    {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError); 
    }
    
    if (waitpid(pid, &status, 0) != pid)
    {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }

    if(output != NULL)
    {
        *output = tempOutput;
        tempOutput = NULL;
    }
    
    if(exitCode != NULL)
        *exitCode = WEXITSTATUS(status);
    else if (status)
    {
        ceError = CENTERROR_COMMAND_FAILED;
        BAIL_ON_CENTERIS_ERROR(ceError); 
    }
    
error:   
    CT_SAFE_FREE_MEMORY(tempOutput);
    return ceError;
}

CENTERROR
CTRunCommand(
    PCSTR command
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    
    int code = system(command);
    
    if (code < 0)
    {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);   
    }
    else if (code > 0)
    {
        ceError = CENTERROR_COMMAND_FAILED;
        BAIL_ON_CENTERIS_ERROR(ceError);
    }
    
error:   
    return ceError;
}

CENTERROR
CTSpawnProcessWithFds(
    PCSTR pszCommand,
    const PSTR* ppszArgs,
    int dwFdIn,
    int dwFdOut,
    int dwFdErr,
    PPROCINFO* ppProcInfo
    )
{
    return CTSpawnProcessWithEnvironment(pszCommand, ppszArgs, NULL, dwFdIn, dwFdOut, dwFdErr, ppProcInfo);
}

CENTERROR
CTSpawnProcessWithEnvironment(
    PCSTR pszCommand,
    const PSTR* ppszArgs,
    const PSTR* ppszEnv,
    int dwFdIn,
    int dwFdOut,
    int dwFdErr,
    PPROCINFO* ppProcInfo
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    PPROCINFO pProcInfo = NULL;
    int pid = -1;
    int iFd = 0;
    int fd0[2] = {-1, -1};
    int fd1[2] = {-1, -1};
    int fd2[2] = {-1, -1};
    int maxfd = 0;
    struct rlimit rlm;

    memset(&rlm, 0, sizeof(struct rlimit));

    if (getrlimit(RLIMIT_NOFILE, &rlm) < 0) {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }

    maxfd = rlm.rlim_max;


    if (dwFdIn >= 0)
    {
        fd0[0] = dup(dwFdIn);
    } else if (pipe(fd0) < 0) {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }

    if (dwFdOut >= 0)
    {
        fd1[1] = dup(dwFdOut);
    } else if (pipe(fd1) < 0) {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }

    if (dwFdErr >= 0)
    {
        fd2[1] = dup(dwFdErr);
    } else if (pipe(fd2) < 0) {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }

    if ((pid = fork()) < 0) {
        ceError = CTMapSystemError(errno);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }

    if (pid > 0) {
        /* parent */
        if (fd0[0] >= 0) close(fd0[0]); fd0[0] = -1;
        if (fd1[1] >= 0) close(fd1[1]); fd1[1] = -1;
        if (fd2[1] >= 0) close(fd2[1]); fd2[1] = -1;

        ceError = CTAllocateMemory(sizeof(PROCINFO), (PVOID*)&pProcInfo);
        BAIL_ON_CENTERIS_ERROR(ceError);

        pProcInfo->pid = pid;
        pProcInfo->fdin = fd0[1];
        pProcInfo->fdout = fd1[0];
        pProcInfo->fderr = fd2[0];

        *ppProcInfo = pProcInfo;
        pProcInfo = NULL;

    }

    if (pid == 0) {

        /* child -- must exit/abort and never bail via goto */
        if (fd0[1] >= 0) close(fd0[1]); fd0[1] = -1;
        if (fd1[0] >= 0) close(fd1[0]); fd1[0] = -1;
        if (fd2[0] >= 0) close(fd2[0]); fd2[0] = -1;

        if (fd0[0] != STDIN_FILENO) {
            if (dup2(fd0[0], STDIN_FILENO) != STDIN_FILENO) {
                abort();
            }
            close(fd0[0]); fd0[0] = -1;
        }

        if (fd1[1] != STDOUT_FILENO) {
            if (dup2(fd1[1], STDOUT_FILENO) != STDOUT_FILENO) {
                abort();
            }
            close(fd1[1]); fd1[1] = -1;
        }

        if (fd2[1] != STDERR_FILENO) {
            if (dup2(fd2[1], STDERR_FILENO) != STDERR_FILENO) {
                abort();
            }
            close(fd2[1]); fd2[1] = -1;
        }

        for (iFd = STDERR_FILENO+1; iFd < 20; iFd++)
            close(iFd);

        if (ppszEnv)
            execve(pszCommand, (char **)ppszArgs, (char **)ppszEnv);
        else
            execvp(pszCommand, (char **)ppszArgs);
        _exit(127);
    }

    return ceError;

error:

    if (pProcInfo)
        CTFreeProcInfo(pProcInfo);

    for(iFd = 0; iFd < 2; iFd++)
        if (fd0[iFd] >= 0)
            close(fd0[iFd]);

    for(iFd = 0; iFd < 2; iFd++)
        if (fd1[iFd] >= 0)
            close(fd1[iFd]);

    for(iFd = 0; iFd < 2; iFd++)
        if (fd2[iFd] >= 0)
            close(fd2[iFd]);

    return ceError;
}

void
CTFreeProcInfo(
    PPROCINFO pProcInfo
    )
{
    if(pProcInfo == NULL)
        return;
    if (pProcInfo->fdin >= 0)
        close(pProcInfo->fdin);
    if (pProcInfo->fdout >= 0)
        close(pProcInfo->fdout);
    if (pProcInfo->fderr >= 0)
        close(pProcInfo->fderr);
    CTFreeMemory(pProcInfo);
}

CENTERROR
CTGetExitStatus(
    PPROCINFO pProcInfo,
    PLONG plstatus
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    int status = 0;

    while(1)
    {
        if (waitpid(pProcInfo->pid, &status, 0) < 0) {
            if (errno == EINTR)
                continue;
            ceError = CTMapSystemError(errno);
            BAIL_ON_CENTERIS_ERROR(ceError);
        }
        else
            break;
    }

    if (WIFEXITED(status)) {
        *plstatus = WEXITSTATUS(status);
    } else {
        BAIL_ON_CENTERIS_ERROR(CENTERROR_ABNORMAL_TERMINATION);
    }

error:
    return ceError;
}

void CTCaptureOutputToExc(
    PCSTR command,
    LWException **exc
    )
{
    PSTR output = NULL;
    CENTERROR ceError = CTCaptureOutputWithStderr(command, TRUE, &output);

    if(ceError == CENTERROR_COMMAND_FAILED)
    {
        PSTR showOutput = output;
        if(showOutput == NULL)
            showOutput = "";
        LW_RAISE_EX(exc, ceError, "Command failed", "The execution of '%s' returned a non-zero exit code. Here is the output from the program:\n%s", command, showOutput);
        goto cleanup;
    }
    LW_CLEANUP_CTERR(exc, ceError);

cleanup:
    CT_SAFE_FREE_STRING(output);
}
