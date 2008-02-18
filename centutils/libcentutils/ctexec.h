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
#ifndef __CT_EXEC_H__
#define __CT_EXEC_H__

#include "ctbase.h"
#include "lwexc.h"

/**
 * @defgroup CTExec Unix program interaction
 */
/*@{*/

typedef struct __PROCINFO
{
    pid_t pid;
    int   fdin;
    int   fdout;
    int   fderr;
} PROCINFO, *PPROCINFO;

/**
 * @brief Capture output of a Unix command
 *
 * Runs the specified Unix command and captures output
 * to a buffer.  The buffer is dynamically allocated and
 * freeing it becomes the responsibility of the caller.
 * The command is passed to the standard Unix shell
 * (/bin/sh), which is reponsible for parsing and executing
 * it; shell features such as pipelines may be used, but
 * care must be taken to properly escape commands. The caller may need to
 * free the output buffer even if the function fails.
 * @see CTEscapeString
 * @param command @in the Unix command to execute
 * @param output @out the dynamically-allocated buffer containing
 * the output of the command
 * @errcode
 * @canfail
 */
CENTERROR
CTCaptureOutput(
    PCSTR command,
    PSTR* output
    );

/**
 * @brief Run the specified unix command and only show the output if it fails
 *
 * Runs the specified Unix command and captures the output. An exception is
 * returned if the command exits with a non-zero exit code. The output is only
 * returned in the exception, otherwise the command runs silently.
 * @see CTEscapeString
 * @param command @in the Unix command to execute
 * @errcode
 * @canfail
 */
void CTCaptureOutputToExc(
    PCSTR command,
    LWException **exc
    );

/**
 * @brief Capture output of a Unix command
 *
 * Runs the specified Unix command and captures output
 * to a buffer.  The buffer is dynamically allocated and
 * freeing it becomes the responsibility of the caller.
 * The command is passed to the standard Unix shell
 * (/bin/sh), which is reponsible for parsing and executing
 * it; shell features such as pipelines may be used, but
 * care must be taken to properly escape commands. The caller may need to
 * free the output buffer even if the function fails.
 * @see CTEscapeString
 * @param command @in the Unix command to execute
 * @param captureStderr @in whether to capture stderr with stdout or to let it output to the caller's stderr.
 * @param output @out the dynamically-allocated buffer containing
 * the output of the command
 * @errcode
 * @canfail
 */
CENTERROR
CTCaptureOutputWithStderr(
    PCSTR command,
    BOOLEAN captureStderr,
    PSTR* output
    );

CENTERROR
CTCaptureOutputWithStderrEx(
    PCSTR command,
    PCSTR* ppszArgs,
    BOOLEAN captureStderr,
    PSTR* output,
    int* exitCode
    );

/**
 * @brief Run a command
 *
 * Runs the specified Unix command and waits for it to
 * complete.  The command is passed to the standard Unix shell
 * (/bin/sh), which is responsible for parsing and executing
 * it; shell features such as pipelines may be used, but
 * care must be taken to properly escape commands.
 *
 * This command returns CENTERROR_COMMAND_FAILED if the program returns a non-
 * zero exit code.
 *
 * @see CTEscapeString
 * @param command @in the Unix command to execute
 * @errcode
 * @canfail
 */
CENTERROR
CTRunCommand(
    PCSTR command
    );

CENTERROR
CTSpawnProcessWithFds(
    PCSTR pszCommand,
    const PSTR* ppszArgs,
    int dwFdIn,
    int dwFdOut,
    int dwFdErr,
    PPROCINFO* ppProcInfo
    );

CENTERROR
CTSpawnProcessWithEnvironment(
    PCSTR pszCommand,
    const PSTR* ppszArgs,
    const PSTR* ppszEnv,
    int dwFdIn,
    int dwFdOut,
    int dwFdErr,
    PPROCINFO* ppProcInfo
    );

void
CTFreeProcInfo(
    PPROCINFO pProcInfo
    );

CENTERROR
CTGetExitStatus(
    PPROCINFO pProcInfo,
    PLONG plstatus
    );
    
/*@}*/

#endif
