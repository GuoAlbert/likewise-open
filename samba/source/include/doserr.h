/* 
   Unix SMB/CIFS implementation.
   DOS error code constants
   Copyright (C) Andrew Tridgell              1992-2000
   Copyright (C) John H Terpstra              1996-2000
   Copyright (C) Luke Kenneth Casson Leighton 1996-2000
   Copyright (C) Paul Ashton                  1998-2000
   Copyright (C) Gerald (Jerry) Carter        2005
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DOSERR_H
#define _DOSERR_H

/* Error classes */

#define ERRDOS 0x01 /*  Error is from the core DOS operating system set. */
#define ERRSRV 0x02  /* Error is generated by the server network file manager.*/
#define ERRHRD 0x03  /* Error is an hardware error. */
#define ERRCMD 0xFF  /* Command was not in the "SMB" format. */

/* SMB X/Open error codes for the ERRDOS error class */
#define ERRsuccess 0 /* No error */
#define ERRbadfunc 1 /* Invalid function (or system call) */
#define ERRbadfile 2 /* File not found (pathname error) */
#define ERRbadpath 3 /* Directory not found */
#define ERRnofids 4 /* Too many open files */
#define ERRnoaccess 5 /* Access denied */
#define ERRbadfid 6 /* Invalid fid */
#define ERRbadmcb 7 /* Memory control blocks destroyed. */
#define ERRnomem 8 /* Out of memory */
#define ERRbadmem 9 /* Invalid memory block address */
#define ERRbadenv 10 /* Invalid environment */
#define ERRbadformat 11 /* Bad Format */
#define ERRbadaccess 12 /* Invalid open mode */
#define ERRbaddata 13 /* Invalid data (only from ioctl call) */
#define ERRres 14 /* reserved */
#define ERRbaddrive 15 /* Invalid drive */
#define ERRremcd 16 /* Attempt to delete current directory */
#define ERRdiffdevice 17 /* rename/move across different filesystems */
#define ERRnofiles 18 /* no more files found in file search */
#define ERRgeneral 31 /* General failure */
#define ERRbadshare 32 /* Share mode on file conflict with open mode */
#define ERRlock 33 /* Lock request conflicts with existing lock */
#define ERRunsup 50 /* Request unsupported, returned by Win 95, RJS 20Jun98 */
#define ERRnetnamedel 64 /* Network name deleted or not available */
#define ERRnosuchshare 67 /* You specified an invalid share name */
#define ERRfilexists 80 /* File in operation already exists */
#define ERRinvalidparam 87
#define ERRcannotopen 110 /* Cannot open the file specified */
#define ERRbufferoverflow 111
#define ERRinsufficientbuffer 122
#define ERRinvalidname 123 /* Invalid name */
#define ERRunknownlevel 124
#define ERRnotlocked 158 /* This region is not locked by this locking context. */
#define ERRinvalidpath 161
#define ERRcancelviolation 173
#define ERRnoatomiclocks 174
#define ERRrename 183
#define ERRbadpipe 230 /* Named pipe invalid */
#define ERRpipebusy 231 /* All instances of pipe are busy */
#define ERRpipeclosing 232 /* named pipe close in progress */
#define ERRnotconnected 233 /* No process on other end of named pipe */
#define ERRmoredata 234 /* More data to be returned */
#define ERReainconsistent 255 /* from EMC */
#define ERRnomoreitems 259
#define ERRbaddirectory 267 /* Invalid directory name in a path. */
#define ERReasnotsupported 282 /* Extended attributes */
#define ERRlogonfailure 1326 /* Unknown username or bad password */
#define ERRbuftoosmall 2123
#define ERRunknownipc 2142
#define ERRnosuchprintjob 2151
#define ERRinvgroup 2455

/* here's a special one from observing NT */
#define ERRnoipc 66 /* don't support ipc */

/* These errors seem to be only returned by the NT printer driver system */
#define ERRdriveralreadyinstalled 1795 /* ERROR_PRINTER_DRIVER_ALREADY_INSTALLED */
#define ERRunknownprinterport 1796 /* ERROR_UNKNOWN_PORT */
#define ERRunknownprinterdriver 1797 /* ERROR_UNKNOWN_PRINTER_DRIVER */
#define ERRunknownprintprocessor 1798 /* ERROR_UNKNOWN_PRINTPROCESSOR */
#define ERRinvalidseparatorfile 1799 /* ERROR_INVALID_SEPARATOR_FILE */
#define ERRinvalidjobpriority 1800 /* ERROR_INVALID_PRIORITY */
#define ERRinvalidprintername 1801 /* ERROR_INVALID_PRINTER_NAME */
#define ERRprinteralreadyexists 1802 /* ERROR_PRINTER_ALREADY_EXISTS */
#define ERRinvalidprintercommand 1803 /* ERROR_INVALID_PRINTER_COMMAND */
#define ERRinvaliddatatype 1804 /* ERROR_INVALID_DATATYPE */
#define ERRinvalidenvironment 1805 /* ERROR_INVALID_ENVIRONMENT */

#define ERRunknownprintmonitor 3000 /* ERROR_UNKNOWN_PRINT_MONITOR */
#define ERRprinterdriverinuse 3001 /* ERROR_PRINTER_DRIVER_IN_USE */
#define ERRspoolfilenotfound 3002 /* ERROR_SPOOL_FILE_NOT_FOUND */
#define ERRnostartdoc 3003 /* ERROR_SPL_NO_STARTDOC */
#define ERRnoaddjob 3004 /* ERROR_SPL_NO_ADDJOB */
#define ERRprintprocessoralreadyinstalled 3005 /* ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED */
#define ERRprintmonitoralreadyinstalled 3006 /* ERROR_PRINT_MONITOR_ALREADY_INSTALLED */
#define ERRinvalidprintmonitor 3007 /* ERROR_INVALID_PRINT_MONITOR */
#define ERRprintmonitorinuse 3008 /* ERROR_PRINT_MONITOR_IN_USE */
#define ERRprinterhasjobsqueued 3009 /* ERROR_PRINTER_HAS_JOBS_QUEUED */

/* Error codes for the ERRSRV class */

#define ERRerror 1 /* Non specific error code */
#define ERRbadpw 2 /* Bad password */
#define ERRbadtype 3 /* reserved */
#define ERRaccess 4 /* No permissions to do the requested operation */
#define ERRinvnid 5 /* tid invalid */
#define ERRinvnetname 6 /* Invalid servername */
#define ERRinvdevice 7 /* Invalid device */
#define ERRqfull 49 /* Print queue full */
#define ERRqtoobig 50 /* Queued item too big */
#define ERRinvpfid 52 /* Invalid print file in smb_fid */
#define ERRsmbcmd 64 /* Unrecognised command */
#define ERRsrverror 65 /* smb server internal error */
#define ERRfilespecs 67 /* fid and pathname invalid combination */
#define ERRbadlink 68 /* reserved */
#define ERRbadpermits 69 /* Access specified for a file is not valid */
#define ERRbadpid 70 /* reserved */
#define ERRsetattrmode 71 /* attribute mode invalid */
#define ERRpaused 81 /* Message server paused */
#define ERRmsgoff 82 /* Not receiving messages */
#define ERRnoroom 83 /* No room for message */
#define ERRrmuns 87 /* too many remote usernames */
#define ERRtimeout 88 /* operation timed out */
#define ERRnoresource  89 /* No resources currently available for request. */
#define ERRtoomanyuids 90 /* too many userids */
#define ERRbaduid 91 /* bad userid */
#define ERRuseMPX 250 /* temporarily unable to use raw mode, use MPX mode */
#define ERRuseSTD 251 /* temporarily unable to use raw mode, use standard mode */
#define ERRcontMPX 252 /* resume MPX mode */
#define ERRbadPW /* reserved */
#define ERRnosupport 0xFFFF
#define ERRunknownsmb 22 /* from NT 3.5 response */

/* Error codes for the ERRHRD class */

#define ERRnowrite 19 /* read only media */
#define ERRbadunit 20 /* Unknown device */
#define ERRnotready 21 /* Drive not ready */
#define ERRbadcmd 22 /* Unknown command */
#define ERRdata 23 /* Data (CRC) error */
#define ERRbadreq 24 /* Bad request structure length */
#define ERRseek 25
#define ERRbadmedia 26
#define ERRbadsector 27
#define ERRnopaper 28
#define ERRwrite 29 /* write fault */
#define ERRread 30 /* read fault */
#define ERRgeneral 31 /* General hardware failure */
#define ERRwrongdisk 34
#define ERRFCBunavail 35
#define ERRsharebufexc 36 /* share buffer exceeded */
#define ERRdiskfull 39


/* these are win32 error codes. There are only a few places where
   these matter for Samba, primarily in the NT printing code */
#define WERR_OK W_ERROR(0)
#define WERR_BADFUNC W_ERROR(1)
#define WERR_BADFILE W_ERROR(2)
#define WERR_ACCESS_DENIED W_ERROR(5)
#define WERR_BADFID W_ERROR(6)
#define WERR_NOMEM W_ERROR(8)
#define WERR_GENERAL_FAILURE W_ERROR(31)
#define WERR_NOT_SUPPORTED W_ERROR(50)
#define WERR_DEVICE_NOT_EXIST W_ERROR(55)
#define WERR_PRINTQ_FULL W_ERROR(61)
#define WERR_NO_SPOOL_SPACE W_ERROR(62)
#define WERR_NO_SUCH_SHARE W_ERROR(67)
#define WERR_ALREADY_EXISTS W_ERROR(80)
#define WERR_BAD_PASSWORD W_ERROR(86)
#define WERR_INVALID_PARAM W_ERROR(87)
#define WERR_SEM_TIMEOUT W_ERROR(121)
#define WERR_INSUFFICIENT_BUFFER W_ERROR(122)
#define WERR_INVALID_NAME W_ERROR(123)
#define WERR_UNKNOWN_LEVEL W_ERROR(124)
#define WERR_OBJECT_PATH_INVALID W_ERROR(161)
#define WERR_NO_MORE_ITEMS W_ERROR(259)
#define WERR_MORE_DATA W_ERROR(234)
#define WERR_INVALID_OWNER W_ERROR(1307)
#define WERR_IO_PENDING W_ERROR(997)
#define WERR_CAN_NOT_COMPLETE W_ERROR(1003)
#define WERR_INVALID_FLAGS W_ERROR(1004)
#define WERR_REG_CORRUPT W_ERROR(1015)
#define WERR_REG_IO_FAILURE W_ERROR(1016)
#define WERR_REG_FILE_INVALID W_ERROR(1017)
#define WERR_NO_SUCH_SERVICE W_ERROR(1060)
#define WERR_INVALID_SERVICE_CONTROL W_ERROR(1052)
#define WERR_SERVICE_ALREADY_RUNNING W_ERROR(1056)
#define WERR_SERVICE_DISABLED W_ERROR(1058)
#define WERR_SERVICE_NEVER_STARTED W_ERROR(1077)
#define WERR_INVALID_COMPUTER_NAME W_ERROR(1210)
#define WERR_MACHINE_LOCKED W_ERROR(1271)
#define WERR_NO_LOGON_SERVERS W_ERROR(1311)
#define WERR_NO_SUCH_LOGON_SESSION W_ERROR(1312)
#define WERR_LOGON_FAILURE W_ERROR(1326)
#define WERR_NO_SUCH_DOMAIN W_ERROR(1355)
#define WERR_INVALID_SECURITY_DESCRIPTOR W_ERROR(1338)
#define WERR_TIME_SKEW W_ERROR(1398)
#define WERR_EVENTLOG_FILE_CORRUPT W_ERROR(1500)
#define WERR_SERVER_UNAVAILABLE W_ERROR(1722)
#define WERR_INVALID_FORM_NAME W_ERROR(1902)
#define WERR_INVALID_FORM_SIZE W_ERROR(1903)
#define WERR_PASSWORD_MUST_CHANGE W_ERROR(1907)
#define WERR_DOMAIN_CONTROLLER_NOT_FOUND W_ERROR(1908)
#define WERR_ACCOUNT_LOCKED_OUT W_ERROR(1909)

/* should these go down to NERR_BASE ? */
#define WERR_BUF_TOO_SMALL W_ERROR(2123)
#define WERR_JOB_NOT_FOUND W_ERROR(2151)
#define WERR_DEST_NOT_FOUND W_ERROR(2152)
#define WERR_USER_EXISTS W_ERROR(2224)
#define WERR_NOT_LOCAL_DOMAIN W_ERROR(2320)
#define WERR_DC_NOT_FOUND W_ERROR(2453)

#define WERR_SETUP_ALREADY_JOINED W_ERROR(2691)
#define WERR_SETUP_NOT_JOINED W_ERROR(2692)
#define WERR_SETUP_DOMAIN_CONTROLLER W_ERROR(2693)
#define WERR_DEFAULT_JOIN_REQUIRED W_ERROR(2694)

#define WERR_DEVICE_NOT_AVAILABLE W_ERROR(4319)
#define WERR_STATUS_MORE_ENTRIES   W_ERROR(0x0105)

#define WERR_PRINTER_DRIVER_ALREADY_INSTALLED W_ERROR(ERRdriveralreadyinstalled)
#define WERR_UNKNOWN_PORT W_ERROR(ERRunknownprinterport)
#define WERR_UNKNOWN_PRINTER_DRIVER W_ERROR(ERRunknownprinterdriver)
#define WERR_UNKNOWN_PRINTPROCESSOR W_ERROR(ERRunknownprintprocessor)
#define WERR_INVALID_SEPARATOR_FILE W_ERROR(ERRinvalidseparatorfile)
#define WERR_INVALID_PRIORITY W_ERROR(ERRinvalidjobpriority)
#define WERR_INVALID_PRINTER_NAME W_ERROR(ERRinvalidprintername)
#define WERR_PRINTER_ALREADY_EXISTS W_ERROR(ERRprinteralreadyexists)
#define WERR_INVALID_PRINTER_COMMAND W_ERROR(ERRinvalidprintercommand)
#define WERR_INVALID_DATATYPE W_ERROR(ERRinvaliddatatype)
#define WERR_INVALID_ENVIRONMENT W_ERROR(ERRinvalidenvironment)

#define WERR_UNKNOWN_PRINT_MONITOR W_ERROR(ERRunknownprintmonitor)
#define WERR_PRINTER_DRIVER_IN_USE W_ERROR(ERRprinterdriverinuse)
#define WERR_SPOOL_FILE_NOT_FOUND W_ERROR(ERRspoolfilenotfound)
#define WERR_SPL_NO_STARTDOC W_ERROR(ERRnostartdoc)
#define WERR_SPL_NO_ADDJOB W_ERROR(ERRnoaddjob)
#define WERR_PRINT_PROCESSOR_ALREADY_INSTALLED W_ERROR(ERRprintprocessoralreadyinstalled)
#define WERR_PRINT_MONITOR_ALREADY_INSTALLED W_ERROR(ERRprintmonitoralreadyinstalled)
#define WERR_INVALID_PRINT_MONITOR W_ERROR(ERRinvalidprintmonitor)
#define WERR_PRINT_MONITOR_IN_USE W_ERROR(ERRprintmonitorinuse)
#define WERR_PRINTER_HAS_JOBS_QUEUED W_ERROR(ERRprinterhasjobsqueued)

/* Configuration Manager Errors */
/* Basically Win32 errors meanings are specific to the \ntsvcs pipe */

#define WERR_CM_NO_MORE_HW_PROFILES W_ERROR(35)
#define WERR_CM_NO_SUCH_VALUE W_ERROR(37)


/* DFS errors */

#ifndef NERR_BASE
#define NERR_BASE (2100)
#endif

#define WERR_DFS_NO_SUCH_VOL            W_ERROR(NERR_BASE+562)
#define WERR_DFS_NO_SUCH_SHARE          W_ERROR(NERR_BASE+565)
#define WERR_DFS_NO_SUCH_SERVER         W_ERROR(NERR_BASE+573)
#define WERR_DFS_INTERNAL_ERROR         W_ERROR(NERR_BASE+590)
#define WERR_DFS_CANT_CREATE_JUNCT      W_ERROR(NERR_BASE+569)

#define WERR_NET_NAME_NOT_FOUND		W_ERROR(NERR_BASE+210)


#endif /* _DOSERR_H */
