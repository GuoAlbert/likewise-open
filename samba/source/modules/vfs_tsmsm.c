/*
  Unix SMB/CIFS implementation.
  Samba VFS module for handling offline files
  with Tivoli Storage Manager Space Management

  (c) Alexander Bokovoy, 2007
  (c) Andrew Tridgell, 2007
  
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
/*
  This VFS module accepts following options:
  tsmsm: hsm script = <path to hsm script> (/bin/true by default, i.e. does nothing)
         hsm script should point to a shell script which accepts two arguments:
	 <operation> <filepath>
	 where <operation> is currently 'offline' to set offline status of the <filepath>

  tsmsm: online ratio = ratio to check reported size against actual file size (0.5 by default)

  The TSMSM VFS module tries to avoid calling expensive DMAPI calls with some heuristics
  based on the fact that number of blocks reported of a file multiplied by 512 will be
  bigger than 'online ratio' of actual size for online (non-migrated) files.

  If checks fail, we call DMAPI and ask for specific IBM attribute which present for
  offline (migrated) files. If this attribute presents, we consider file offline.
 */

#include "includes.h"

#ifndef USE_DMAPI
#error "This module requires DMAPI support!"
#endif

#ifdef HAVE_XFS_DMAPI_H
#include <xfs/dmapi.h>
#elif defined(HAVE_SYS_DMI_H)
#include <sys/dmi.h>
#elif defined(HAVE_SYS_JFSDMAPI_H)
#include <sys/jfsdmapi.h>
#elif defined(HAVE_SYS_DMAPI_H)
#include <sys/dmapi.h>
#elif defined(HAVE_DMAPI_H)
#include <dmapi.h>
#endif

#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE 
#endif

#include <math.h> 

/* optimisation tunables - used to avoid the DMAPI slow path */
#define FILE_IS_ONLINE_RATIO      0.5
#define DM_ATTRIB_OBJECT "IBMObj"
#define DM_ATTRIB_MIGRATED "IBMMig"

struct tsmsm_struct {
	dm_sessid_t sid;
	float online_ratio;
	char *hsmscript;
};

#define TSM_STRINGIFY(a) #a
#define TSM_TOSTRING(a) TSM_STRINGIFY(a)

static void tsmsm_free_data(void **pptr) {
	struct tsmsm_struct **tsmd = (struct tsmsm_struct **)pptr;
	if(!tsmd) return;
	TALLOC_FREE(*tsmd);
}

static int tsmsm_connect(struct vfs_handle_struct *handle,
			 const char *service,
			 const char *user) {
	struct tsmsm_struct *tsmd = TALLOC_ZERO_P(handle, struct tsmsm_struct);
	const char *hsmscript, *tsmname;
	const char *fres;
	
	if (!tsmd) {
		DEBUG(0,("tsmsm_connect: out of memory!\n"));
		return -1;
	}

	tsmd->sid = *(dm_sessid_t*) dmapi_get_current_session();

	if (tsmd->sid == DM_NO_SESSION) {
		DEBUG(0,("tsmsm_connect: no DMAPI session for Samba is available!\n"));
		TALLOC_FREE(tsmd);
		return -1;
	}

	tsmname = (handle->param ? handle->param : "tsmsm");
	hsmscript = lp_parm_const_string(SNUM(handle->conn), tsmname,
					 "hsm script", NULL);
	if (hsmscript) {
		tsmd->hsmscript = talloc_strdup(tsmd, hsmscript);
		if(!tsmd->hsmscript) {
			DEBUG(1, ("tsmsm_connect: can't allocate memory for hsm script path"));
			TALLOC_FREE(tsmd);
			return -1;
		}
	} else {
		DEBUG(1, ("tsmsm_connect: can't call hsm script because it "
			  "is not set to anything in the smb.conf\n"
			  "Use %s: 'hsm script = path' to set it\n",
			  tsmname));
		TALLOC_FREE(tsmd);
		return -1;
	}

	fres = lp_parm_const_string(SNUM(handle->conn), tsmname, 
				    "online ratio", TSM_TOSTRING(FILE_IS_ONLINE_RATIO));
	tsmd->online_ratio = strtof(fres, NULL);
	if((tsmd->online_ratio == (float)0) || ((errno == ERANGE) &&
						((tsmd->online_ratio == HUGE_VALF) ||
						 (tsmd->online_ratio == HUGE_VALL)))) {
		DEBUG(1, ("tsmsm_connect: error while getting online ratio from smb.conf."
			  "Default to %s.\n", TSM_TOSTRING(FILE_IS_ONLINE_RATIO)));
		tsmd->online_ratio = FILE_IS_ONLINE_RATIO;
	}

        /* Store the private data. */
        SMB_VFS_HANDLE_SET_DATA(handle, tsmd, tsmsm_free_data,
                                struct tsmsm_struct, return -1);
        return SMB_VFS_NEXT_CONNECT(handle, service, user); 
}

static bool tsmsm_is_offline(struct vfs_handle_struct *handle, 
			    const char *path,
			    SMB_STRUCT_STAT *stbuf) {
	struct tsmsm_struct *tsmd = (struct tsmsm_struct *) handle->data;
	void *dmhandle = NULL;
	size_t dmhandle_len = 0;
	size_t rlen;
	dm_attrname_t dmname;
	int ret;
	bool offline;

        /* if the file has more than FILE_IS_ONLINE_RATIO of blocks available,
	   then assume it is not offline (it may not be 100%, as it could be sparse) */
	if (512 * (off_t)stbuf->st_blocks >= stbuf->st_size * tsmd->online_ratio) {
		DEBUG(10,("%s not offline: st_blocks=%ld st_size=%ld online_ratio=%.2f\n", 
			  path, stbuf->st_blocks, stbuf->st_size, tsmd->online_ratio));
		return false;
	}
	
        /* using POSIX capabilities does not work here. It's a slow path, so 
	 * become_root() is just as good anyway (tridge) 
	 */

	/* Also, AIX has DMAPI but no POSIX capablities support. In this case,
	 * we need to be root to do DMAPI manipulations.
	 */
	become_root();

	/* go the slow DMAPI route */
	if (dm_path_to_handle((char*)path, &dmhandle, &dmhandle_len) != 0) {
		DEBUG(2,("dm_path_to_handle failed - assuming offline (%s) - %s\n", 
			 path, strerror(errno)));
		offline = true;
		goto done;
	}

	memset(&dmname, 0, sizeof(dmname));
	strlcpy((char *)&dmname.an_chars[0], DM_ATTRIB_OBJECT, sizeof(dmname.an_chars));

	ret = dm_get_dmattr(tsmd->sid, dmhandle, dmhandle_len, 
			    DM_NO_TOKEN, &dmname, 0, NULL, &rlen);

	/* its offline if the IBMObj attribute exists */
	offline = (ret == 0 || (ret == -1 && errno == E2BIG));

	DEBUG(10,("dm_get_dmattr %s ret=%d (%s)\n", path, ret, strerror(errno)));

	ret = 0;

	dm_handle_free(dmhandle, dmhandle_len);	

done:
	unbecome_root();
	return offline;
}


static bool tsmsm_aio_force(struct vfs_handle_struct *handle, struct files_struct *fsp)
{
	SMB_STRUCT_STAT sbuf;
	struct tsmsm_struct *tsmd = (struct tsmsm_struct *) handle->data;
	/* see if the file might be offline. This is called before each IO
	   to ensure we use AIO if the file is offline. We don't do the full dmapi
	   call as that would be too slow, instead we err on the side of using AIO
	   if the file might be offline
	*/
	if(SMB_VFS_FSTAT(fsp, &sbuf) == 0) {
		DEBUG(10,("tsmsm_aio_force st_blocks=%ld st_size=%ld online_ratio=%.2f\n", 
			  sbuf.st_blocks, sbuf.st_size, tsmd->online_ratio));
		return !(512 * (off_t)sbuf.st_blocks >= sbuf.st_size * tsmd->online_ratio);
	}
	return false;
}

static ssize_t tsmsm_aio_return(struct vfs_handle_struct *handle, struct files_struct *fsp, 
				SMB_STRUCT_AIOCB *aiocb)
{
	ssize_t result;

	result = SMB_VFS_NEXT_AIO_RETURN(handle, fsp, aiocb);
	if(result >= 0) {
		notify_fname(handle->conn, NOTIFY_ACTION_MODIFIED,
			     FILE_NOTIFY_CHANGE_ATTRIBUTES,
			     fsp->fsp_name);
	}

	return result;
}

static ssize_t tsmsm_sendfile(vfs_handle_struct *handle, int tofd, files_struct *fsp, const DATA_BLOB *hdr,
			      SMB_OFF_T offset, size_t n)
{
	bool file_online = tsmsm_aio_force(handle, fsp);

	if(!file_online) 
	    return ENOSYS;
	    
	return SMB_VFS_NEXT_SENDFILE(handle, tofd, fsp, hdr, offset, n);
}

/* We do overload pread to allow notification when file becomes online after offline status */
/* We don't intercept SMB_VFS_READ here because all file I/O now goes through SMB_VFS_PREAD instead */
static ssize_t tsmsm_pread(struct vfs_handle_struct *handle, struct files_struct *fsp, 
			   void *data, size_t n, SMB_OFF_T offset) {
	ssize_t result;
	bool notify_online = tsmsm_aio_force(handle, fsp);

	result = SMB_VFS_NEXT_PREAD(handle, fsp, data, n, offset);
	if((result != -1) && notify_online) {
	    /* We can't actually force AIO at this point (came here not from reply_read_and_X) 
	       what we can do is to send notification that file became online
	    */
		notify_fname(handle->conn, NOTIFY_ACTION_MODIFIED,
			     FILE_NOTIFY_CHANGE_ATTRIBUTES,
			     fsp->fsp_name);
	}

	return result;
}

static ssize_t tsmsm_pwrite(struct vfs_handle_struct *handle, struct files_struct *fsp, 
			   void *data, size_t n, SMB_OFF_T offset) {
	ssize_t result;
	bool notify_online = tsmsm_aio_force(handle, fsp);

	result = SMB_VFS_NEXT_PWRITE(handle, fsp, data, n, offset);
	if((result != -1) && notify_online) {
	    /* We can't actually force AIO at this point (came here not from reply_read_and_X) 
	       what we can do is to send notification that file became online
	    */
		notify_fname(handle->conn, NOTIFY_ACTION_MODIFIED,
			     FILE_NOTIFY_CHANGE_ATTRIBUTES,
			     fsp->fsp_name);
	}

	return result;
}

static int tsmsm_set_offline(struct vfs_handle_struct *handle, 
			     const char *path) {
	struct tsmsm_struct *tsmd = (struct tsmsm_struct *) handle->data;
	int result = 0;
	char *command;

	/* Now, call the script */
	command = talloc_asprintf(tsmd, "%s offline \"%s\"", tsmd->hsmscript, path);
	if(!command) {
		DEBUG(1, ("tsmsm_set_offline: can't allocate memory to run hsm script"));
		return -1;
	}
	DEBUG(10, ("tsmsm_set_offline: Running [%s]\n", command));
	if((result = smbrun(command, NULL)) != 0) {
		DEBUG(1,("tsmsm_set_offline: Running [%s] returned %d\n", command, result));
	}
	TALLOC_FREE(command);
	return result;
}

static bool tsmsm_statvfs(struct vfs_handle_struct *handle,  const char *path, vfs_statvfs_struct *statbuf)
{
	bool result;

	result = SMB_VFS_NEXT_STATVFS(handle, path, statbuf);
	statbuf->FsCapabilities | = FILE_SUPPORTS_REMOTE_STORAGE | FILE_SUPPORTS_REPARSE_POINTS;

	return result;
}

static vfs_op_tuple vfs_tsmsm_ops[] = {

	/* Disk operations */

	{SMB_VFS_OP(tsmsm_connect),	SMB_VFS_OP_CONNECT,
	 SMB_VFS_LAYER_TRANSPARENT},
	{SMB_VFS_OP(tsmsm_statvfs),	SMB_VFS_OP_STATVFS,
	 SMB_VFS_LAYER_TRANSPARENT},
	{SMB_VFS_OP(tsmsm_aio_force),	SMB_VFS_OP_AIO_FORCE,
	 SMB_VFS_LAYER_TRANSPARENT},
	{SMB_VFS_OP(tsmsm_aio_return),	SMB_VFS_OP_AIO_RETURN,
	 SMB_VFS_LAYER_TRANSPARENT},
	{SMB_VFS_OP(tsmsm_pread),	SMB_VFS_OP_PREAD,
	 SMB_VFS_LAYER_TRANSPARENT},
	{SMB_VFS_OP(tsmsm_pwrite),	SMB_VFS_OP_PWRITE,
	 SMB_VFS_LAYER_TRANSPARENT},
	{SMB_VFS_OP(tsmsm_sendfile),	SMB_VFS_OP_SENDFILE,
	 SMB_VFS_LAYER_TRANSPARENT},
	{SMB_VFS_OP(tsmsm_is_offline),	SMB_VFS_OP_IS_OFFLINE,
	 SMB_VFS_LAYER_OPAQUE},
	{SMB_VFS_OP(tsmsm_set_offline),	SMB_VFS_OP_SET_OFFLINE,
	 SMB_VFS_LAYER_OPAQUE},

	/* Finish VFS operations definition */

	{SMB_VFS_OP(NULL),		SMB_VFS_OP_NOOP,
	 SMB_VFS_LAYER_NOOP}
};

NTSTATUS vfs_tsmsm_init(void);
NTSTATUS vfs_tsmsm_init(void)
{
	return smb_register_vfs(SMB_VFS_INTERFACE_VERSION,
				"tsmsm", vfs_tsmsm_ops);
}
