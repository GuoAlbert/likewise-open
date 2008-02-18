/* 
   Unix SMB/CIFS implementation.
   Samba Version functions
   
   Copyright (C) Stefan Metzmacher	2003
   
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

#include "includes.h"
#include <assert.h>

const char *samba_version_string(void)
{
#ifndef SAMBA_VERSION_VENDOR_SUFFIX
	return SAMBA_VERSION_OFFICIAL_STRING;
#else
	static char *samba_version;
	int res;
#ifdef SAMBA_VERSION_VENDOR_PATCH
	char *tmp_version;
#endif

	if (samba_version != NULL)
		return samba_version;

	res = asprintf(&samba_version, "%s-%s",
		       SAMBA_VERSION_OFFICIAL_STRING,
		       SAMBA_VERSION_VENDOR_SUFFIX);
	/*
	 * Can't use smb_panic here due to dependencies
	 */
	assert(res != -1);

#ifdef SAMBA_VERSION_VENDOR_PATCH
	res = asprintf(&tmp_version, "%s-%d", samba_version,
		       SAMBA_VERSION_VENDOR_PATCH);
	/*
	 * Can't use smb_panic here due to dependencies
	 */
	assert(res != -1);

	SAFE_FREE(samba_version);

	samba_version = tmp_version;
#endif

	return samba_version;
#endif
}


/* Reuse the Samba Vendor suffix with the Likewikse Version */

const char *likewise_version_string(void)
{
#ifndef SAMBA_VERSION_VENDOR_SUFFIX
	return LIKEWISE_VERSION_OFFICIAL_STRING;
#else
	static fstring samba_version;
	static bool init_samba_version;
#ifdef SAMBA_VERSION_VENDOR_PATCH
	fstring tmp_version;
	size_t remaining;
#endif

	if (init_samba_version)
		return samba_version;

	snprintf(samba_version,sizeof(samba_version),"%s-%s",
		LIKEWISE_VERSION_OFFICIAL_STRING,
		SAMBA_VERSION_VENDOR_SUFFIX);

#ifdef SAMBA_VERSION_VENDOR_PATCH
	remaining = sizeof(samba_version)-strlen(samba_version);
	snprintf( tmp_version, sizeof(tmp_version),  "-%d", SAMBA_VERSION_VENDOR_PATCH );
	strlcat( samba_version, tmp_version, remaining-1 );
#endif

	init_samba_version = True;
	return samba_version;
#endif
}

