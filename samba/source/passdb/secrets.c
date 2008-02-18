/*
   Unix SMB/CIFS implementation.
   Copyright (C) Andrew Tridgell 1992-2001
   Copyright (C) Andrew Bartlett      2002
   Copyright (C) Rafal Szczesniak     2002
   Copyright (C) Tim Potter           2001

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

/* the Samba secrets database stores any generated, private information
   such as the local SID and machine trust password */

#include "includes.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_PASSDB

static TDB_CONTEXT *tdb;

/* Urrrg. global.... */
bool global_machine_password_needs_changing;

/**
 * Use a TDB to store an incrementing random seed.
 *
 * Initialised to the current pid, the very first time Samba starts,
 * and incremented by one each time it is needed.
 *
 * @note Not called by systems with a working /dev/urandom.
 */
static void get_rand_seed(int *new_seed)
{
	*new_seed = sys_getpid();
	if (tdb) {
		tdb_change_int32_atomic(tdb, "INFO/random_seed", new_seed, 1);
	}
}

/* open up the secrets database */
bool secrets_init(void)
{
	TALLOC_CTX *ctx;
	char *fname = NULL;
	unsigned char dummy;

	if (tdb)
		return True;

	ctx = talloc_init("secrets_init");
	if (!ctx) {
		return false;
	}
	fname = talloc_asprintf(ctx,
			"%s/secrets.tdb",
			lp_private_dir());
	if (!fname) {
		TALLOC_FREE(ctx);
		return false;
	}

	tdb = tdb_open_log(fname, 0, TDB_DEFAULT, O_RDWR|O_CREAT, 0600);

	if (!tdb) {
		DEBUG(0,("Failed to open %s\n", fname));
		TALLOC_FREE(ctx);
		return False;
	}

	TALLOC_FREE(ctx);

	/**
	 * Set a reseed function for the crypto random generator
	 *
	 * This avoids a problem where systems without /dev/urandom
	 * could send the same challenge to multiple clients
	 */
	set_rand_reseed_callback(get_rand_seed);

	/* Ensure that the reseed is done now, while we are root, etc */
	generate_random_buffer(&dummy, sizeof(dummy));

	return True;
}

/*
 * close secrets.tdb
 */
void secrets_shutdown(void)
{
	if (!tdb) {
		return;
	}

	tdb_close(tdb);
	tdb = NULL;
}

/* read a entry from the secrets database - the caller must free the result
   if size is non-null then the size of the entry is put in there
 */
void *secrets_fetch(const char *key, size_t *size)
{
	TDB_DATA dbuf;
	secrets_init();
	if (!tdb)
		return NULL;
	dbuf = tdb_fetch(tdb, string_tdb_data(key));
	if (size)
		*size = dbuf.dsize;
	return dbuf.dptr;
}

/* store a secrets entry
 */
bool secrets_store(const char *key, const void *data, size_t size)
{
	secrets_init();
	if (!tdb)
		return False;
	return tdb_trans_store(tdb, string_tdb_data(key),
			       make_tdb_data((const uint8 *)data, size),
			       TDB_REPLACE) == 0;
}


/* delete a secets database entry
 */
bool secrets_delete(const char *key)
{
	secrets_init();
	if (!tdb)
		return False;
	return tdb_trans_delete(tdb, string_tdb_data(key)) == 0;
}

/**
 * Form a key for fetching the domain sid
 *
 * @param domain domain name
 *
 * @return keystring
 **/
static const char *domain_sid_keystr(const char *domain)
{
	char *keystr;

	keystr = talloc_asprintf(talloc_tos(), "%s/%s",
				 SECRETS_DOMAIN_SID, domain);
	SMB_ASSERT(keystr != NULL);

	strupper_m(keystr);

	return keystr;
}

bool secrets_store_domain_sid(const char *domain, const DOM_SID *sid)
{
	bool ret;

	ret = secrets_store(domain_sid_keystr(domain), sid, sizeof(DOM_SID));

	/* Force a re-query, in case we modified our domain */
	if (ret)
		reset_global_sam_sid();
	return ret;
}

bool secrets_fetch_domain_sid(const char *domain, DOM_SID *sid)
{
	DOM_SID *dyn_sid;
	size_t size = 0;

	dyn_sid = (DOM_SID *)secrets_fetch(domain_sid_keystr(domain), &size);

	if (dyn_sid == NULL)
		return False;

	if (size != sizeof(DOM_SID)) {
		SAFE_FREE(dyn_sid);
		return False;
	}

	*sid = *dyn_sid;
	SAFE_FREE(dyn_sid);
	return True;
}

bool secrets_store_domain_guid(const char *domain, struct GUID *guid)
{
	fstring key;

	slprintf(key, sizeof(key)-1, "%s/%s", SECRETS_DOMAIN_GUID, domain);
	strupper_m(key);
	return secrets_store(key, guid, sizeof(struct GUID));
}

bool secrets_fetch_domain_guid(const char *domain, struct GUID *guid)
{
	struct GUID *dyn_guid;
	fstring key;
	size_t size = 0;
	struct GUID new_guid;

	slprintf(key, sizeof(key)-1, "%s/%s", SECRETS_DOMAIN_GUID, domain);
	strupper_m(key);
	dyn_guid = (struct GUID *)secrets_fetch(key, &size);

	if (!dyn_guid) {
		if (lp_server_role() == ROLE_DOMAIN_PDC) {
			smb_uuid_generate_random(&new_guid);
			if (!secrets_store_domain_guid(domain, &new_guid))
				return False;
			dyn_guid = (struct GUID *)secrets_fetch(key, &size);
		}
		if (dyn_guid == NULL) {
			return False;
		}
	}

	if (size != sizeof(struct GUID)) {
		DEBUG(1,("UUID size %d is wrong!\n", (int)size));
		SAFE_FREE(dyn_guid);
		return False;
	}

	*guid = *dyn_guid;
	SAFE_FREE(dyn_guid);
	return True;
}

/**
 * Form a key for fetching the machine trust account sec channel type
 *
 * @param domain domain name
 *
 * @return keystring
 **/
static const char *machine_sec_channel_type_keystr(const char *domain)
{
	char *keystr;

	keystr = talloc_asprintf(talloc_tos(), "%s/%s",
				 SECRETS_MACHINE_SEC_CHANNEL_TYPE, domain);
	SMB_ASSERT(keystr != NULL);

	strupper_m(keystr);

	return keystr;
}

/**
 * Form a key for fetching the machine trust account last change time
 *
 * @param domain domain name
 *
 * @return keystring
 **/
static const char *machine_last_change_time_keystr(const char *domain)
{
	char *keystr;

	keystr = talloc_asprintf(talloc_tos(), "%s/%s",
				 SECRETS_MACHINE_LAST_CHANGE_TIME, domain);
	SMB_ASSERT(keystr != NULL);

	strupper_m(keystr);

	return keystr;
}


/**
 * Form a key for fetching the machine trust account password
 *
 * @param domain domain name
 *
 * @return keystring
 **/
static const char *machine_password_keystr(const char *domain)
{
	char *keystr;

	keystr = talloc_asprintf(talloc_tos(), "%s/%s",
				 SECRETS_MACHINE_PASSWORD, domain);
	SMB_ASSERT(keystr != NULL);

	strupper_m(keystr);

	return keystr;
}

/**
 * Form a key for fetching the machine trust account password
 *
 * @param domain domain name
 *
 * @return stored password's key
 **/
static const char *trust_keystr(const char *domain)
{
	char *keystr;

	keystr = talloc_asprintf(talloc_tos(), "%s/%s",
				 SECRETS_MACHINE_ACCT_PASS, domain);
	SMB_ASSERT(keystr != NULL);

	strupper_m(keystr);

	return keystr;
}

/**
 * Form a key for fetching a trusted domain password
 *
 * @param domain trusted domain name
 *
 * @return stored password's key
 **/
static char *trustdom_keystr(const char *domain)
{
	char *keystr;

	keystr = talloc_asprintf(talloc_tos(), "%s/%s",
				 SECRETS_DOMTRUST_ACCT_PASS, domain);
	SMB_ASSERT(keystr != NULL);
	strupper_m(keystr);

	return keystr;
}

/************************************************************************
 Lock the trust password entry.
************************************************************************/

bool secrets_lock_trust_account_password(const char *domain, bool dolock)
{
	if (!tdb)
		return False;

	if (dolock)
		return (tdb_lock_bystring(tdb, trust_keystr(domain)) == 0);
	else
		tdb_unlock_bystring(tdb, trust_keystr(domain));
	return True;
}

/************************************************************************
 Routine to get the default secure channel type for trust accounts
************************************************************************/

uint32 get_default_sec_channel(void)
{
	if (lp_server_role() == ROLE_DOMAIN_BDC ||
	    lp_server_role() == ROLE_DOMAIN_PDC) {
		return SEC_CHAN_BDC;
	} else {
		return SEC_CHAN_WKSTA;
	}
}

/************************************************************************
 Routine to get the trust account password for a domain.
 This only tries to get the legacy hashed version of the password.
 The user of this function must have locked the trust password file using
 the above secrets_lock_trust_account_password().
************************************************************************/

bool secrets_fetch_trust_account_password_legacy(const char *domain,
						 uint8 ret_pwd[16],
						 time_t *pass_last_set_time,
						 uint32 *channel)
{
	struct machine_acct_pass *pass;
	size_t size = 0;

	if (!(pass = (struct machine_acct_pass *)secrets_fetch(
		      trust_keystr(domain), &size))) {
		DEBUG(5, ("secrets_fetch failed!\n"));
		return False;
	}

	if (size != sizeof(*pass)) {
		DEBUG(0, ("secrets were of incorrect size!\n"));
		return False;
	}

	if (pass_last_set_time) {
		*pass_last_set_time = pass->mod_time;
	}
	memcpy(ret_pwd, pass->hash, 16);

	if (channel) {
		*channel = get_default_sec_channel();
	}

	/* Test if machine password has expired and needs to be changed */
	if (lp_machine_password_timeout()) {
		if (pass->mod_time > 0 && time(NULL) > (pass->mod_time +
				(time_t)lp_machine_password_timeout())) {
			global_machine_password_needs_changing = True;
		}
	}

	SAFE_FREE(pass);
	return True;
}

/************************************************************************
 Routine to get the trust account password for a domain.
 The user of this function must have locked the trust password file using
 the above secrets_lock_trust_account_password().
************************************************************************/

bool secrets_fetch_trust_account_password(const char *domain, uint8 ret_pwd[16],
					  time_t *pass_last_set_time,
					  uint32 *channel)
{
	char *plaintext;

	plaintext = secrets_fetch_machine_password(domain, pass_last_set_time,
						   channel);
	if (plaintext) {
		DEBUG(4,("Using cleartext machine password\n"));
		E_md4hash(plaintext, ret_pwd);
		SAFE_FREE(plaintext);
		return True;
	}

	return secrets_fetch_trust_account_password_legacy(domain, ret_pwd,
							   pass_last_set_time,
							   channel);
}

/**
 * Pack SID passed by pointer
 *
 * @param pack_buf pointer to buffer which is to be filled with packed data
 * @param bufsize size of packing buffer
 * @param sid pointer to sid to be packed
 *
 * @return length of the packed representation of the whole structure
 **/
static size_t tdb_sid_pack(uint8 *pack_buf, int bufsize, DOM_SID* sid)
{
	int idx;
	size_t len = 0;
	uint8 *p = pack_buf;
	int remaining_space = pack_buf ? bufsize : 0;

	if (!sid) {
		return -1;
	}

	len += tdb_pack(p, remaining_space, "bb", sid->sid_rev_num,
	                sid->num_auths);
	if (pack_buf) {
		p = pack_buf + len;
		remaining_space = bufsize - len;
	}

	for (idx = 0; idx < 6; idx++) {
		len += tdb_pack(p, remaining_space, "b",
				sid->id_auth[idx]);
		if (pack_buf) {
			p = pack_buf + len;
			remaining_space = bufsize - len;
		}
	}

	for (idx = 0; idx < MAXSUBAUTHS; idx++) {
		len += tdb_pack(p, remaining_space, "d",
				sid->sub_auths[idx]);
		if (pack_buf) {
			p = pack_buf + len;
			remaining_space = bufsize - len;
		}
	}

	return len;
}

/**
 * Unpack SID into a pointer
 *
 * @param pack_buf pointer to buffer with packed representation
 * @param bufsize size of the buffer
 * @param sid pointer to sid structure to be filled with unpacked data
 *
 * @return size of structure unpacked from buffer
 **/
static size_t tdb_sid_unpack(uint8 *pack_buf, int bufsize, DOM_SID* sid)
{
	int idx, len = 0;

	if (!sid || !pack_buf) return -1;

	len += tdb_unpack(pack_buf + len, bufsize - len, "bb",
	                  &sid->sid_rev_num, &sid->num_auths);

	for (idx = 0; idx < 6; idx++) {
		len += tdb_unpack(pack_buf + len, bufsize - len, "b",
				  &sid->id_auth[idx]);
	}

	for (idx = 0; idx < MAXSUBAUTHS; idx++) {
		len += tdb_unpack(pack_buf + len, bufsize - len, "d",
				  &sid->sub_auths[idx]);
	}

	return len;
}

/**
 * Pack TRUSTED_DOM_PASS passed by pointer
 *
 * @param pack_buf pointer to buffer which is to be filled with packed data
 * @param bufsize size of the buffer
 * @param pass pointer to trusted domain password to be packed
 *
 * @return length of the packed representation of the whole structure
 **/
static size_t tdb_trusted_dom_pass_pack(uint8 *pack_buf, int bufsize,
					TRUSTED_DOM_PASS* pass)
{
	int idx, len = 0;
	uint8 *p = pack_buf;
	int remaining_space = pack_buf ? bufsize : 0;

	if (!pass) {
		return -1;
	}

	/* packing unicode domain name and password */
	len += tdb_pack(p, remaining_space, "d",
			pass->uni_name_len);
	if (pack_buf) {
		p = pack_buf + len;
		remaining_space = bufsize - len;
	}

	for (idx = 0; idx < 32; idx++) {
		len += tdb_pack(p, remaining_space, "w",
				 pass->uni_name[idx]);
		if (pack_buf) {
			p = pack_buf + len;
			remaining_space = bufsize - len;
		}
	}

	len += tdb_pack(p, remaining_space, "dPd", pass->pass_len,
	                     pass->pass, pass->mod_time);
	if (pack_buf) {
		p = pack_buf + len;
		remaining_space = bufsize - len;
	}

	/* packing SID structure */
	len += tdb_sid_pack(p, remaining_space, &pass->domain_sid);
	if (pack_buf) {
		p = pack_buf + len;
		remaining_space = bufsize - len;
	}

	return len;
}


/**
 * Unpack TRUSTED_DOM_PASS passed by pointer
 *
 * @param pack_buf pointer to buffer with packed representation
 * @param bufsize size of the buffer
 * @param pass pointer to trusted domain password to be filled with unpacked data
 *
 * @return size of structure unpacked from buffer
 **/
static size_t tdb_trusted_dom_pass_unpack(uint8 *pack_buf, int bufsize,
					  TRUSTED_DOM_PASS* pass)
{
	int idx, len = 0;
	char *passp = NULL;

	if (!pack_buf || !pass) return -1;

	/* unpack unicode domain name and plaintext password */
	len += tdb_unpack(pack_buf, bufsize - len, "d", &pass->uni_name_len);

	for (idx = 0; idx < 32; idx++)
		len +=  tdb_unpack(pack_buf + len, bufsize - len, "w",
				   &pass->uni_name[idx]);

	len += tdb_unpack(pack_buf + len, bufsize - len, "dPd",
			  &pass->pass_len, &passp, &pass->mod_time);
	if (passp) {
		fstrcpy(pass->pass, passp);
	}
	SAFE_FREE(passp);

	/* unpack domain sid */
	len += tdb_sid_unpack(pack_buf + len, bufsize - len,
			      &pass->domain_sid);

	return len;
}

/************************************************************************
 Routine to get account password to trusted domain
************************************************************************/

bool secrets_fetch_trusted_domain_password(const char *domain, char** pwd,
                                           DOM_SID *sid, time_t *pass_last_set_time)
{
	struct trusted_dom_pass pass;
	size_t size = 0;

	/* unpacking structures */
	uint8 *pass_buf;
	int pass_len = 0;

	ZERO_STRUCT(pass);

	/* fetching trusted domain password structure */
	if (!(pass_buf = (uint8 *)secrets_fetch(trustdom_keystr(domain),
					       &size))) {
		DEBUG(5, ("secrets_fetch failed!\n"));
		return False;
	}

	/* unpack trusted domain password */
	pass_len = tdb_trusted_dom_pass_unpack(pass_buf, size, &pass);
	SAFE_FREE(pass_buf);

	if (pass_len != size) {
		DEBUG(5, ("Invalid secrets size. Unpacked data doesn't match trusted_dom_pass structure.\n"));
		return False;
	}

	/* the trust's password */
	if (pwd) {
		*pwd = SMB_STRDUP(pass.pass);
		if (!*pwd) {
			return False;
		}
	}

	/* last change time */
	if (pass_last_set_time) *pass_last_set_time = pass.mod_time;

	/* domain sid */
	if (sid != NULL) sid_copy(sid, &pass.domain_sid);

	return True;
}

/**
 * Routine to store the password for trusted domain
 *
 * @param domain remote domain name
 * @param pwd plain text password of trust relationship
 * @param sid remote domain sid
 *
 * @return true if succeeded
 **/

bool secrets_store_trusted_domain_password(const char* domain, const char* pwd,
                                           const DOM_SID *sid)
{
	smb_ucs2_t *uni_dom_name;
	bool ret;

	/* packing structures */
	uint8 *pass_buf = NULL;
	int pass_len = 0;

	struct trusted_dom_pass pass;
	ZERO_STRUCT(pass);

	if (push_ucs2_allocate(&uni_dom_name, domain) == (size_t)-1) {
		DEBUG(0, ("Could not convert domain name %s to unicode\n",
			  domain));
		return False;
	}

	strncpy_w(pass.uni_name, uni_dom_name, sizeof(pass.uni_name) - 1);
	pass.uni_name_len = strlen_w(uni_dom_name)+1;
	SAFE_FREE(uni_dom_name);

	/* last change time */
	pass.mod_time = time(NULL);

	/* password of the trust */
	pass.pass_len = strlen(pwd);
	fstrcpy(pass.pass, pwd);

	/* domain sid */
	sid_copy(&pass.domain_sid, sid);

	/* Calculate the length. */
	pass_len = tdb_trusted_dom_pass_pack(NULL, 0, &pass);
	pass_buf = SMB_MALLOC_ARRAY(uint8, pass_len);
	if (!pass_buf) {
		return false;
	}
	pass_len = tdb_trusted_dom_pass_pack(pass_buf, pass_len, &pass);
	ret = secrets_store(trustdom_keystr(domain), (void *)pass_buf,
			pass_len);
	SAFE_FREE(pass_buf);
	return ret;
}

/************************************************************************
 Routine to delete the plaintext machine account password
************************************************************************/

bool secrets_delete_machine_password(const char *domain)
{
	return secrets_delete(machine_password_keystr(domain));
}

/************************************************************************
 Routine to delete the plaintext machine account password, sec channel type and
 last change time from secrets database
************************************************************************/

bool secrets_delete_machine_password_ex(const char *domain)
{
	if (!secrets_delete(machine_password_keystr(domain))) {
		return false;
	}
	if (!secrets_delete(machine_sec_channel_type_keystr(domain))) {
		return false;
	}
	return secrets_delete(machine_last_change_time_keystr(domain));
}

/************************************************************************
 Routine to delete the domain sid
************************************************************************/

bool secrets_delete_domain_sid(const char *domain)
{
	return secrets_delete(domain_sid_keystr(domain));
}

/************************************************************************
 Routine to set the plaintext machine account password for a realm
the password is assumed to be a null terminated ascii string
************************************************************************/

bool secrets_store_machine_password(const char *pass, const char *domain, uint32 sec_channel)
{
	bool ret;
	uint32 last_change_time;
	uint32 sec_channel_type;

	ret = secrets_store(machine_password_keystr(domain), pass, strlen(pass)+1);
	if (!ret)
		return ret;

	SIVAL(&last_change_time, 0, time(NULL));
	ret = secrets_store(machine_last_change_time_keystr(domain), &last_change_time, sizeof(last_change_time));

	SIVAL(&sec_channel_type, 0, sec_channel);
	ret = secrets_store(machine_sec_channel_type_keystr(domain), &sec_channel_type, sizeof(sec_channel_type));

	return ret;
}

/************************************************************************
 Routine to fetch the plaintext machine account password for a realm
 the password is assumed to be a null terminated ascii string.
************************************************************************/

char *secrets_fetch_machine_password(const char *domain,
				     time_t *pass_last_set_time,
				     uint32 *channel)
{
	char *ret;
	ret = (char *)secrets_fetch(machine_password_keystr(domain), NULL);

	if (pass_last_set_time) {
		size_t size;
		uint32 *last_set_time;
		last_set_time = (unsigned int *)secrets_fetch(machine_last_change_time_keystr(domain), &size);
		if (last_set_time) {
			*pass_last_set_time = IVAL(last_set_time,0);
			SAFE_FREE(last_set_time);
		} else {
			*pass_last_set_time = 0;
		}
	}

	if (channel) {
		size_t size;
		uint32 *channel_type;
		channel_type = (unsigned int *)secrets_fetch(machine_sec_channel_type_keystr(domain), &size);
		if (channel_type) {
			*channel = IVAL(channel_type,0);
			SAFE_FREE(channel_type);
		} else {
			*channel = get_default_sec_channel();
		}
	}

	return ret;
}

/************************************************************************
 Routine to delete the password for trusted domain
************************************************************************/

bool trusted_domain_password_delete(const char *domain)
{
	return secrets_delete(trustdom_keystr(domain));
}

bool secrets_store_ldap_pw(const char* dn, char* pw)
{
	char *key = NULL;
	bool ret;

	if (asprintf(&key, "%s/%s", SECRETS_LDAP_BIND_PW, dn) < 0) {
		DEBUG(0, ("secrets_store_ldap_pw: asprintf failed!\n"));
		return False;
	}

	ret = secrets_store(key, pw, strlen(pw)+1);

	SAFE_FREE(key);
	return ret;
}

/*******************************************************************
 Find the ldap password.
******************************************************************/

bool fetch_ldap_pw(char **dn, char** pw)
{
	char *key = NULL;
	size_t size = 0;

	*dn = smb_xstrdup(lp_ldap_admin_dn());

	if (asprintf(&key, "%s/%s", SECRETS_LDAP_BIND_PW, *dn) < 0) {
		SAFE_FREE(*dn);
		DEBUG(0, ("fetch_ldap_pw: asprintf failed!\n"));
	}

	*pw=(char *)secrets_fetch(key, &size);
	SAFE_FREE(key);

	if (!size) {
		/* Upgrade 2.2 style entry */
		char *p;
	        char* old_style_key = SMB_STRDUP(*dn);
		char *data;
		fstring old_style_pw;

		if (!old_style_key) {
			DEBUG(0, ("fetch_ldap_pw: strdup failed!\n"));
			return False;
		}

		for (p=old_style_key; *p; p++)
			if (*p == ',') *p = '/';

		data=(char *)secrets_fetch(old_style_key, &size);
		if (!size && size < sizeof(old_style_pw)) {
			DEBUG(0,("fetch_ldap_pw: neither ldap secret retrieved!\n"));
			SAFE_FREE(old_style_key);
			SAFE_FREE(*dn);
			return False;
		}

		size = MIN(size, sizeof(fstring)-1);
		strncpy(old_style_pw, data, size);
		old_style_pw[size] = 0;

		SAFE_FREE(data);

		if (!secrets_store_ldap_pw(*dn, old_style_pw)) {
			DEBUG(0,("fetch_ldap_pw: ldap secret could not be upgraded!\n"));
			SAFE_FREE(old_style_key);
			SAFE_FREE(*dn);
			return False;
		}
		if (!secrets_delete(old_style_key)) {
			DEBUG(0,("fetch_ldap_pw: old ldap secret could not be deleted!\n"));
		}

		SAFE_FREE(old_style_key);

		*pw = smb_xstrdup(old_style_pw);
	}

	return True;
}

/**
 * Get trusted domains info from secrets.tdb.
 **/

NTSTATUS secrets_trusted_domains(TALLOC_CTX *mem_ctx, uint32 *num_domains,
				 struct trustdom_info ***domains)
{
	TDB_LIST_NODE *keys, *k;
	char *pattern;
	TALLOC_CTX *tmp_ctx;

	if (!(tmp_ctx = talloc_new(mem_ctx))) {
		return NT_STATUS_NO_MEMORY;
	}

	if (!secrets_init()) return NT_STATUS_ACCESS_DENIED;

	/* generate searching pattern */
	pattern = talloc_asprintf(tmp_ctx, "%s/*", SECRETS_DOMTRUST_ACCT_PASS);
	if (pattern == NULL) {
		DEBUG(0, ("secrets_trusted_domains: talloc_asprintf() "
			  "failed!\n"));
		TALLOC_FREE(tmp_ctx);
		return NT_STATUS_NO_MEMORY;
	}

	*num_domains = 0;

	/*
	 * Make sure that a talloc context for the trustdom_info structs
	 * exists
	 */

	if (!(*domains = TALLOC_ARRAY(mem_ctx, struct trustdom_info *, 1))) {
		TALLOC_FREE(tmp_ctx);
		return NT_STATUS_NO_MEMORY;
	}

	/* fetching trusted domains' data and collecting them in a list */
	keys = tdb_search_keys(tdb, pattern);

	/* searching for keys in secrets db -- way to go ... */
	for (k = keys; k; k = k->next) {
		uint8 *packed_pass;
		size_t size = 0, packed_size = 0;
		struct trusted_dom_pass pass;
		char *secrets_key;
		struct trustdom_info *dom_info;

		/* important: ensure null-termination of the key string */
		secrets_key = talloc_strndup(tmp_ctx,
					     (const char *)k->node_key.dptr,
					     k->node_key.dsize);
		if (!secrets_key) {
			DEBUG(0, ("strndup failed!\n"));
			tdb_search_list_free(keys);
			TALLOC_FREE(tmp_ctx);
			return NT_STATUS_NO_MEMORY;
		}

		packed_pass = (uint8 *)secrets_fetch(secrets_key, &size);
		packed_size = tdb_trusted_dom_pass_unpack(packed_pass, size,
							  &pass);
		/* packed representation isn't needed anymore */
		SAFE_FREE(packed_pass);

		if (size != packed_size) {
			DEBUG(2, ("Secrets record %s is invalid!\n",
				  secrets_key));
			continue;
		}

		if (pass.domain_sid.num_auths != 4) {
			DEBUG(0, ("SID %s is not a domain sid, has %d "
				  "auths instead of 4\n",
				  sid_string_dbg(&pass.domain_sid),
				  pass.domain_sid.num_auths));
			continue;
		}

		if (!(dom_info = TALLOC_P(*domains, struct trustdom_info))) {
			DEBUG(0, ("talloc failed\n"));
			tdb_search_list_free(keys);
			TALLOC_FREE(tmp_ctx);
			return NT_STATUS_NO_MEMORY;
		}

		if (pull_ucs2_talloc(dom_info, &dom_info->name,
				     pass.uni_name) == (size_t)-1) {
			DEBUG(2, ("pull_ucs2_talloc failed\n"));
			tdb_search_list_free(keys);
			TALLOC_FREE(tmp_ctx);
			return NT_STATUS_NO_MEMORY;
		}

		sid_copy(&dom_info->sid, &pass.domain_sid);

		ADD_TO_ARRAY(*domains, struct trustdom_info *, dom_info,
			     domains, num_domains);

		if (*domains == NULL) {
			tdb_search_list_free(keys);
			TALLOC_FREE(tmp_ctx);
			return NT_STATUS_NO_MEMORY;
		}
	}

	DEBUG(5, ("secrets_get_trusted_domains: got %d domains\n",
		  *num_domains));

	/* free the results of searching the keys */
	tdb_search_list_free(keys);
	TALLOC_FREE(tmp_ctx);

	return NT_STATUS_OK;
}

/*******************************************************************************
 Lock the secrets tdb based on a string - this is used as a primitive form of mutex
 between smbd instances.
*******************************************************************************/

bool secrets_named_mutex(const char *name, unsigned int timeout)
{
	int ret = 0;

	if (!secrets_init())
		return False;

	ret = tdb_lock_bystring_with_timeout(tdb, name, timeout);
	if (ret == 0)
		DEBUG(10,("secrets_named_mutex: got mutex for %s\n", name ));

	return (ret == 0);
}

/*******************************************************************************
 Unlock a named mutex.
*******************************************************************************/

void secrets_named_mutex_release(const char *name)
{
	tdb_unlock_bystring(tdb, name);
	DEBUG(10,("secrets_named_mutex: released mutex for %s\n", name ));
}

/*******************************************************************************
 Store a complete AFS keyfile into secrets.tdb.
*******************************************************************************/

bool secrets_store_afs_keyfile(const char *cell, const struct afs_keyfile *keyfile)
{
	fstring key;

	if ((cell == NULL) || (keyfile == NULL))
		return False;

	if (ntohl(keyfile->nkeys) > SECRETS_AFS_MAXKEYS)
		return False;

	slprintf(key, sizeof(key)-1, "%s/%s", SECRETS_AFS_KEYFILE, cell);
	return secrets_store(key, keyfile, sizeof(struct afs_keyfile));
}

/*******************************************************************************
 Fetch the current (highest) AFS key from secrets.tdb
*******************************************************************************/
bool secrets_fetch_afs_key(const char *cell, struct afs_key *result)
{
	fstring key;
	struct afs_keyfile *keyfile;
	size_t size = 0;
	uint32 i;

	slprintf(key, sizeof(key)-1, "%s/%s", SECRETS_AFS_KEYFILE, cell);

	keyfile = (struct afs_keyfile *)secrets_fetch(key, &size);

	if (keyfile == NULL)
		return False;

	if (size != sizeof(struct afs_keyfile)) {
		SAFE_FREE(keyfile);
		return False;
	}

	i = ntohl(keyfile->nkeys);

	if (i > SECRETS_AFS_MAXKEYS) {
		SAFE_FREE(keyfile);
		return False;
	}

	*result = keyfile->entry[i-1];

	result->kvno = ntohl(result->kvno);

	return True;
}

/******************************************************************************
  When kerberos is not available, choose between anonymous or
  authenticated connections.

  We need to use an authenticated connection if DCs have the
  RestrictAnonymous registry entry set > 0, or the "Additional
  restrictions for anonymous connections" set in the win2k Local
  Security Policy.

  Caller to free() result in domain, username, password
*******************************************************************************/
void secrets_fetch_ipc_userpass(char **username, char **domain, char **password)
{
	*username = (char *)secrets_fetch(SECRETS_AUTH_USER, NULL);
	*domain = (char *)secrets_fetch(SECRETS_AUTH_DOMAIN, NULL);
	*password = (char *)secrets_fetch(SECRETS_AUTH_PASSWORD, NULL);

	if (*username && **username) {

		if (!*domain || !**domain)
			*domain = smb_xstrdup(lp_workgroup());

		if (!*password || !**password)
			*password = smb_xstrdup("");

		DEBUG(3, ("IPC$ connections done by user %s\\%s\n",
			  *domain, *username));

	} else {
		DEBUG(3, ("IPC$ connections done anonymously\n"));
		*username = smb_xstrdup("");
		*domain = smb_xstrdup("");
		*password = smb_xstrdup("");
	}
}

/******************************************************************************
 Open or create the schannel session store tdb.
*******************************************************************************/

static TDB_CONTEXT *open_schannel_session_store(TALLOC_CTX *mem_ctx)
{
	TDB_DATA vers;
	uint32 ver;
	TDB_CONTEXT *tdb_sc = NULL;
	char *fname = talloc_asprintf(mem_ctx, "%s/schannel_store.tdb", lp_private_dir());

	if (!fname) {
		return NULL;
	}

        tdb_sc = tdb_open_log(fname, 0, TDB_DEFAULT, O_RDWR|O_CREAT, 0600);

        if (!tdb_sc) {
                DEBUG(0,("open_schannel_session_store: Failed to open %s\n", fname));
		TALLOC_FREE(fname);
                return NULL;
        }

	vers = tdb_fetch_bystring(tdb_sc, "SCHANNEL_STORE_VERSION");
	if (vers.dptr == NULL) {
		/* First opener, no version. */
		SIVAL(&ver,0,1);
		vers.dptr = (uint8 *)&ver;
		vers.dsize = 4;
		tdb_store_bystring(tdb_sc, "SCHANNEL_STORE_VERSION", vers, TDB_REPLACE);
		vers.dptr = NULL;
	} else if (vers.dsize == 4) {
		ver = IVAL(vers.dptr,0);
		if (ver != 1) {
			tdb_close(tdb_sc);
			tdb_sc = NULL;
			DEBUG(0,("open_schannel_session_store: wrong version number %d in %s\n",
				(int)ver, fname ));
		}
	} else {
		tdb_close(tdb_sc);
		tdb_sc = NULL;
		DEBUG(0,("open_schannel_session_store: wrong version number size %d in %s\n",
			(int)vers.dsize, fname ));
	}

	SAFE_FREE(vers.dptr);
	TALLOC_FREE(fname);

	return tdb_sc;
}

/******************************************************************************
 Store the schannel state after an AUTH2 call.
 Note we must be root here.
*******************************************************************************/

bool secrets_store_schannel_session_info(TALLOC_CTX *mem_ctx,
				const char *remote_machine,
				const struct dcinfo *pdc)
{
	TDB_CONTEXT *tdb_sc = NULL;
	TDB_DATA value;
	bool ret;
	char *keystr = talloc_asprintf(mem_ctx, "%s/%s", SECRETS_SCHANNEL_STATE,
				remote_machine);
	if (!keystr) {
		return False;
	}

	strupper_m(keystr);

	/* Work out how large the record is. */
	value.dsize = tdb_pack(NULL, 0, "dBBBBBfff",
				pdc->sequence,
				8, pdc->seed_chal.data,
				8, pdc->clnt_chal.data,
				8, pdc->srv_chal.data,
				16, pdc->sess_key,
				16, pdc->mach_pw,
				pdc->mach_acct,
				pdc->remote_machine,
				pdc->domain);

	value.dptr = TALLOC_ARRAY(mem_ctx, uint8, value.dsize);
	if (!value.dptr) {
		TALLOC_FREE(keystr);
		return False;
	}

	value.dsize = tdb_pack(value.dptr, value.dsize, "dBBBBBfff",
				pdc->sequence,
				8, pdc->seed_chal.data,
				8, pdc->clnt_chal.data,
				8, pdc->srv_chal.data,
				16, pdc->sess_key,
				16, pdc->mach_pw,
				pdc->mach_acct,
				pdc->remote_machine,
				pdc->domain);

	tdb_sc = open_schannel_session_store(mem_ctx);
	if (!tdb_sc) {
		TALLOC_FREE(keystr);
		TALLOC_FREE(value.dptr);
		return False;
	}

	ret = (tdb_store_bystring(tdb_sc, keystr, value, TDB_REPLACE) == 0 ? True : False);

	DEBUG(3,("secrets_store_schannel_session_info: stored schannel info with key %s\n",
		keystr ));

	tdb_close(tdb_sc);
	TALLOC_FREE(keystr);
	TALLOC_FREE(value.dptr);
	return ret;
}

/******************************************************************************
 Restore the schannel state on a client reconnect.
 Note we must be root here.
*******************************************************************************/

bool secrets_restore_schannel_session_info(TALLOC_CTX *mem_ctx,
				const char *remote_machine,
				struct dcinfo **ppdc)
{
	TDB_CONTEXT *tdb_sc = NULL;
	TDB_DATA value;
	unsigned char *pseed_chal = NULL;
	unsigned char *pclnt_chal = NULL;
	unsigned char *psrv_chal = NULL;
	unsigned char *psess_key = NULL;
	unsigned char *pmach_pw = NULL;
	uint32 l1, l2, l3, l4, l5;
	int ret;
	struct dcinfo *pdc = NULL;
	char *keystr = talloc_asprintf(mem_ctx, "%s/%s", SECRETS_SCHANNEL_STATE,
				remote_machine);

	*ppdc = NULL;

	if (!keystr) {
		return False;
	}

	strupper_m(keystr);

	tdb_sc = open_schannel_session_store(mem_ctx);
	if (!tdb_sc) {
		TALLOC_FREE(keystr);
		return False;
	}

	value = tdb_fetch_bystring(tdb_sc, keystr);
	if (!value.dptr) {
		DEBUG(0,("secrets_restore_schannel_session_info: Failed to find entry with key %s\n",
			keystr ));
		tdb_close(tdb_sc);
		return False;
	}

	pdc = TALLOC_ZERO_P(mem_ctx, struct dcinfo);

	/* Retrieve the record. */
	ret = tdb_unpack(value.dptr, value.dsize, "dBBBBBfff",
				&pdc->sequence,
				&l1, &pseed_chal,
				&l2, &pclnt_chal,
				&l3, &psrv_chal,
				&l4, &psess_key,
				&l5, &pmach_pw,
				&pdc->mach_acct,
				&pdc->remote_machine,
				&pdc->domain);

	if (ret == -1 || l1 != 8 || l2 != 8 || l3 != 8 || l4 != 16 || l5 != 16) {
		/* Bad record - delete it. */
		tdb_delete_bystring(tdb_sc, keystr);
		tdb_close(tdb_sc);
		TALLOC_FREE(keystr);
		TALLOC_FREE(pdc);
		SAFE_FREE(pseed_chal);
		SAFE_FREE(pclnt_chal);
		SAFE_FREE(psrv_chal);
		SAFE_FREE(psess_key);
		SAFE_FREE(pmach_pw);
		SAFE_FREE(value.dptr);
		return False;
	}

	tdb_close(tdb_sc);

	memcpy(pdc->seed_chal.data, pseed_chal, 8);
	memcpy(pdc->clnt_chal.data, pclnt_chal, 8);
	memcpy(pdc->srv_chal.data, psrv_chal, 8);
	memcpy(pdc->sess_key, psess_key, 16);
	memcpy(pdc->mach_pw, pmach_pw, 16);

	/* We know these are true so didn't bother to store them. */
	pdc->challenge_sent = True;
	pdc->authenticated = True;

	DEBUG(3,("secrets_restore_schannel_session_info: restored schannel info key %s\n",
		keystr ));

	SAFE_FREE(pseed_chal);
	SAFE_FREE(pclnt_chal);
	SAFE_FREE(psrv_chal);
	SAFE_FREE(psess_key);
	SAFE_FREE(pmach_pw);

	TALLOC_FREE(keystr);
	SAFE_FREE(value.dptr);

	*ppdc = pdc;

	return True;
}

bool secrets_store_generic(const char *owner, const char *key, const char *secret)
{
	char *tdbkey = NULL;
	bool ret;

	if (asprintf(&tdbkey, "SECRETS/GENERIC/%s/%s", owner, key) < 0) {
		DEBUG(0, ("asprintf failed!\n"));
		return False;
	}

	ret = secrets_store(tdbkey, secret, strlen(secret)+1);

	SAFE_FREE(tdbkey);
	return ret;
}

/*******************************************************************
 Find the ldap password.
******************************************************************/

char *secrets_fetch_generic(const char *owner, const char *key)
{
	char *secret = NULL;
	char *tdbkey = NULL;

	if (( ! owner) || ( ! key)) {
		DEBUG(1, ("Invalid Paramters"));
		return NULL;
	}

	if (asprintf(&tdbkey, "SECRETS/GENERIC/%s/%s", owner, key) < 0) {
		DEBUG(0, ("Out of memory!\n"));
		return NULL;
	}

	secret = (char *)secrets_fetch(tdbkey, NULL);
	SAFE_FREE(tdbkey);

	return secret;
}

