/*
   Unix SMB/CIFS implementation.
   ID Mapping Cache

   based on gencache

   Copyright (C) Simo Sorce		2006
   Copyright (C) Rafal Szczesniak	2002

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "includes.h"
#include "winbindd.h"

#define TIMEOUT_LEN 12
#define IDMAP_CACHE_DATA_FMT	"%12u/%s"
#define IDMAP_READ_CACHE_DATA_FMT_TEMPLATE "%%12u/%%%us"

struct idmap_cache_ctx {
	TDB_CONTEXT *tdb;
};

static int idmap_cache_destructor(struct idmap_cache_ctx *cache)
{
	int ret = 0;

	if (cache && cache->tdb) {
		ret = tdb_close(cache->tdb);
		cache->tdb = NULL;
	}

	return ret;
}

struct idmap_cache_ctx *idmap_cache_init(TALLOC_CTX *memctx)
{
	struct idmap_cache_ctx *cache;
	char* cache_fname = NULL;

	cache = talloc(memctx, struct idmap_cache_ctx);
	if ( ! cache) {
		DEBUG(0, ("Out of memory!\n"));
		return NULL;
	}

	cache_fname = lock_path("idmap_cache.tdb");

	DEBUG(10, ("Opening cache file at %s\n", cache_fname));

	cache->tdb = tdb_open_log(cache_fname, 0, TDB_DEFAULT, O_RDWR|O_CREAT, 0600);

	if (!cache->tdb) {
		DEBUG(5, ("Attempt to open %s has failed.\n", cache_fname));
		return NULL;
	}

	talloc_set_destructor(cache, idmap_cache_destructor);

	return cache;
}

void idmap_cache_shutdown(struct idmap_cache_ctx *cache)
{
	talloc_free(cache);
}

NTSTATUS idmap_cache_build_sidkey(TALLOC_CTX *ctx, char **sidkey, const struct id_map *id)
{
	fstring sidstr;

	*sidkey = talloc_asprintf(ctx, "IDMAP/SID/%s",
				  sid_to_fstring(sidstr, id->sid));
	if ( ! *sidkey) {
		DEBUG(1, ("failed to build sidkey, OOM?\n"));
		return NT_STATUS_NO_MEMORY;
	}

	return NT_STATUS_OK;
}

NTSTATUS idmap_cache_build_idkey(TALLOC_CTX *ctx, char **idkey, const struct id_map *id)
{
	*idkey = talloc_asprintf(ctx, "IDMAP/%s/%llu",
				(id->xid.type==ID_TYPE_UID)?"UID":"GID",
				(unsigned long long)id->xid.id);
	if ( ! *idkey) {
		DEBUG(1, ("failed to build idkey, OOM?\n"));
		return NT_STATUS_NO_MEMORY;
	}

	return NT_STATUS_OK;
}

NTSTATUS idmap_cache_set(struct idmap_cache_ctx *cache, const struct id_map *id)
{
	NTSTATUS ret;
	time_t timeout = time(NULL) + lp_idmap_cache_time();
	TDB_DATA databuf;
	char *sidkey;
	char *idkey;
	char *valstr;

	/* Don't cache lookups in the S-1-22-{1,2} domain */
	if ( (id->xid.type == ID_TYPE_UID) &&
	     sid_check_is_in_unix_users(id->sid) )
	{
		return NT_STATUS_OK;
	}
	if ( (id->xid.type == ID_TYPE_GID) &&
	     sid_check_is_in_unix_groups(id->sid) )
	{
		return NT_STATUS_OK;
	}


	ret = idmap_cache_build_sidkey(cache, &sidkey, id);
	if (!NT_STATUS_IS_OK(ret)) return ret;

	/* use sidkey as the local memory ctx */
	ret = idmap_cache_build_idkey(sidkey, &idkey, id);
	if (!NT_STATUS_IS_OK(ret)) {
		goto done;
	}

	/* save SID -> ID */

	/* use sidkey as the local memory ctx */
	valstr = talloc_asprintf(sidkey, IDMAP_CACHE_DATA_FMT, (int)timeout, idkey);
	if (!valstr) {
		DEBUG(0, ("Out of memory!\n"));
		ret = NT_STATUS_NO_MEMORY;
		goto done;
	}

	databuf = string_term_tdb_data(valstr);
	DEBUG(10, ("Adding cache entry with key = %s; value = %s and timeout ="
	           " %s (%d seconds %s)\n", sidkey, valstr , ctime(&timeout),
		   (int)(timeout - time(NULL)),
		   timeout > time(NULL) ? "ahead" : "in the past"));

	if (tdb_store_bystring(cache->tdb, sidkey, databuf, TDB_REPLACE) != 0) {
		DEBUG(3, ("Failed to store cache entry!\n"));
		ret = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	/* save ID -> SID */

	/* use sidkey as the local memory ctx */
	valstr = talloc_asprintf(sidkey, IDMAP_CACHE_DATA_FMT, (int)timeout, sidkey);
	if (!valstr) {
		DEBUG(0, ("Out of memory!\n"));
		ret = NT_STATUS_NO_MEMORY;
		goto done;
	}

	databuf = string_term_tdb_data(valstr);
	DEBUG(10, ("Adding cache entry with key = %s; value = %s and timeout ="
	           " %s (%d seconds %s)\n", idkey, valstr, ctime(&timeout),
		   (int)(timeout - time(NULL)),
		   timeout > time(NULL) ? "ahead" : "in the past"));

	if (tdb_store_bystring(cache->tdb, idkey, databuf, TDB_REPLACE) != 0) {
		DEBUG(3, ("Failed to store cache entry!\n"));
		ret = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	ret = NT_STATUS_OK;

done:
	talloc_free(sidkey);
	return ret;
}

NTSTATUS idmap_cache_set_negative_sid(struct idmap_cache_ctx *cache, const struct id_map *id)
{
	NTSTATUS ret;
	time_t timeout = time(NULL) + lp_idmap_negative_cache_time();
	TDB_DATA databuf;
	char *sidkey;
	char *valstr;

	ret = idmap_cache_build_sidkey(cache, &sidkey, id);
	if (!NT_STATUS_IS_OK(ret)) return ret;

	/* use sidkey as the local memory ctx */
	valstr = talloc_asprintf(sidkey, IDMAP_CACHE_DATA_FMT, (int)timeout, "IDMAP/NEGATIVE");
	if (!valstr) {
		DEBUG(0, ("Out of memory!\n"));
		ret = NT_STATUS_NO_MEMORY;
		goto done;
	}

	databuf = string_term_tdb_data(valstr);
	DEBUG(10, ("Adding cache entry with key = %s; value = %s and timeout ="
	           " %s (%d seconds %s)\n", sidkey, valstr, ctime(&timeout),
		   (int)(timeout - time(NULL)),
		   timeout > time(NULL) ? "ahead" : "in the past"));

	if (tdb_store_bystring(cache->tdb, sidkey, databuf, TDB_REPLACE) != 0) {
		DEBUG(3, ("Failed to store cache entry!\n"));
		ret = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

done:
	talloc_free(sidkey);
	return ret;
}

NTSTATUS idmap_cache_set_negative_id(struct idmap_cache_ctx *cache, const struct id_map *id)
{
	NTSTATUS ret;
	time_t timeout = time(NULL) + lp_idmap_negative_cache_time();
	TDB_DATA databuf;
	char *idkey;
	char *valstr;

	ret = idmap_cache_build_idkey(cache, &idkey, id);
	if (!NT_STATUS_IS_OK(ret)) return ret;

	/* use idkey as the local memory ctx */
	valstr = talloc_asprintf(idkey, IDMAP_CACHE_DATA_FMT, (int)timeout, "IDMAP/NEGATIVE");
	if (!valstr) {
		DEBUG(0, ("Out of memory!\n"));
		ret = NT_STATUS_NO_MEMORY;
		goto done;
	}

	databuf = string_term_tdb_data(valstr);
	DEBUG(10, ("Adding cache entry with key = %s; value = %s and timeout ="
	           " %s (%d seconds %s)\n", idkey, valstr, ctime(&timeout),
		   (int)(timeout - time(NULL)),
		   timeout > time(NULL) ? "ahead" : "in the past"));

	if (tdb_store_bystring(cache->tdb, idkey, databuf, TDB_REPLACE) != 0) {
		DEBUG(3, ("Failed to store cache entry!\n"));
		ret = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

done:
	talloc_free(idkey);
	return ret;
}

NTSTATUS idmap_cache_fill_map(struct id_map *id, const char *value)
{
	char *rem;

	/* see if it is a sid */
	if ( ! strncmp("IDMAP/SID/", value, 10)) {

		if ( ! string_to_sid(id->sid, &value[10])) {
			goto failed;
		}

		id->status = ID_MAPPED;

		return NT_STATUS_OK;
	}

	/* not a SID see if it is an UID or a GID */
	if ( ! strncmp("IDMAP/UID/", value, 10)) {

		/* a uid */
		id->xid.type = ID_TYPE_UID;

	} else if ( ! strncmp("IDMAP/GID/", value, 10)) {

		/* a gid */
		id->xid.type = ID_TYPE_GID;

	} else {

		/* a completely bogus value bail out */
		goto failed;
	}

	id->xid.id = strtoll(&value[10], &rem, 0);
	if (*rem != '\0') {
		goto failed;
	}

	id->status = ID_MAPPED;

	return NT_STATUS_OK;

failed:
	DEBUG(1, ("invalid value: %s\n", value));
	id->status = ID_UNKNOWN;
	return NT_STATUS_INTERNAL_DB_CORRUPTION;
}

bool idmap_cache_is_negative(const char *val)
{
	if ( ! strcmp("IDMAP/NEGATIVE", val)) {
		return True;
	}
	return False;
}

/* search the cahce for the SID an return a mapping if found *
 *
 * 4 cases are possible
 *
 * 1 map found
 * 	in this case id->status = ID_MAPPED and NT_STATUS_OK is returned
 * 2 map not found
 * 	in this case id->status = ID_UNKNOWN and NT_STATUS_NONE_MAPPED is returned
 * 3 negative cache found
 * 	in this case id->status = ID_UNMAPPED and NT_STATUS_OK is returned
 * 4 map found but timer expired
 *      in this case id->status = ID_EXPIRED and NT_STATUS_SYNCHRONIZATION_REQUIRED
 *      is returned. In this case revalidation of the cache is needed.
 */

NTSTATUS idmap_cache_map_sid(struct idmap_cache_ctx *cache, struct id_map *id)
{
	NTSTATUS ret;
	TDB_DATA databuf;
	time_t t;
	char *sidkey;
	char *endptr;
	struct winbindd_domain *our_domain = find_our_domain();
	time_t now = time(NULL);

	/* make sure it is marked as not mapped by default */
	id->status = ID_UNKNOWN;

	ret = idmap_cache_build_sidkey(cache, &sidkey, id);
	if (!NT_STATUS_IS_OK(ret)) return ret;

	databuf = tdb_fetch_bystring(cache->tdb, sidkey);

	if (databuf.dptr == NULL) {
		DEBUG(10, ("Cache entry with key = %s couldn't be found\n", sidkey));
		ret = NT_STATUS_NONE_MAPPED;
		goto done;
	}

	t = strtoll((const char *)databuf.dptr, &endptr, 10);

	if ((endptr == NULL) || (*endptr != '/')) {
		DEBUG(2, ("Invalid gencache data format: %s\n", (const char *)databuf.dptr));
		/* remove the entry */
		tdb_delete_bystring(cache->tdb, sidkey);
		ret = NT_STATUS_NONE_MAPPED;
		goto done;
	}

	/* check it is not negative */
	if (strcmp("IDMAP/NEGATIVE", endptr+1) != 0) {

		DEBUG(10, ("Returning %s cache entry: key = %s, value = %s, "
			   "timeout = %s", t > now ? "valid" :
			   "expired", sidkey, endptr+1, ctime(&t)));

		/* this call if successful will also mark the entry as mapped */
		ret = idmap_cache_fill_map(id, endptr+1);
		if ( ! NT_STATUS_IS_OK(ret)) {
			/* if not valid form delete the entry */
			tdb_delete_bystring(cache->tdb, sidkey);
			ret = NT_STATUS_NONE_MAPPED;
			goto done;
		}

		/* here ret == NT_STATUS_OK and id->status = ID_MAPPED */

		if (t <= now) {
			/* If we've been told to be offline - stay in
			   that state... */
			if ( IS_DOMAIN_OFFLINE(our_domain) ) {
				DEBUG(10,("idmap_cache_map_sid: idmap is offline\n"));
				goto done;
			}

			/* We're expired, set an error code
			   for upper layer */
			ret = NT_STATUS_SYNCHRONIZATION_REQUIRED;
		}

		goto done;
	}

	/* Was a negative cache hit */

	/* Ignore the negative cache when offline */

	if ( IS_DOMAIN_OFFLINE(our_domain) ) {
		DEBUG(10,("idmap_cache_map_sid: idmap is offline\n"));
		goto done;
	}


	/* Check for valid or expired cache hits */
		if (t <= now) {
		/* We're expired. Return not mapped */
			ret = NT_STATUS_NONE_MAPPED;
		} else {
			/* this is not mapped as it was a negative cache hit */
			id->status = ID_UNMAPPED;
			ret = NT_STATUS_OK;
		}

done:
	SAFE_FREE(databuf.dptr);
	talloc_free(sidkey);
	return ret;
}

/* search the cahce for the ID an return a mapping if found *
 *
 * 4 cases are possible
 *
 * 1 map found
 * 	in this case id->status = ID_MAPPED and NT_STATUS_OK is returned
 * 2 map not found
 * 	in this case id->status = ID_UNKNOWN and NT_STATUS_NONE_MAPPED is returned
 * 3 negative cache found
 * 	in this case id->status = ID_UNMAPPED and NT_STATUS_OK is returned
 * 4 map found but timer expired
 *      in this case id->status = ID_EXPIRED and NT_STATUS_SYNCHRONIZATION_REQUIRED
 *      is returned. In this case revalidation of the cache is needed.
 */

NTSTATUS idmap_cache_map_id(struct idmap_cache_ctx *cache, struct id_map *id)
{
	NTSTATUS ret;
	TDB_DATA databuf;
	time_t t;
	char *idkey;
	char *endptr;
	struct winbindd_domain *our_domain = find_our_domain();
	time_t now = time(NULL);

	/* make sure it is marked as unknown by default */
	id->status = ID_UNKNOWN;

	ret = idmap_cache_build_idkey(cache, &idkey, id);
	if (!NT_STATUS_IS_OK(ret)) return ret;

	databuf = tdb_fetch_bystring(cache->tdb, idkey);

	if (databuf.dptr == NULL) {
		DEBUG(10, ("Cache entry with key = %s couldn't be found\n", idkey));
		ret = NT_STATUS_NONE_MAPPED;
		goto done;
	}

	t = strtoll((const char *)databuf.dptr, &endptr, 10);

	if ((endptr == NULL) || (*endptr != '/')) {
		DEBUG(2, ("Invalid gencache data format: %s\n", (const char *)databuf.dptr));
		/* remove the entry */
		tdb_delete_bystring(cache->tdb, idkey);
		ret = NT_STATUS_NONE_MAPPED;
		goto done;
	}

	/* check it is not negative */
	if (strcmp("IDMAP/NEGATIVE", endptr+1) != 0) {

		DEBUG(10, ("Returning %s cache entry: key = %s, value = %s, "
			   "timeout = %s", t > now ? "valid" :
			   "expired", idkey, endptr+1, ctime(&t)));

		/* this call if successful will also mark the entry as mapped */
		ret = idmap_cache_fill_map(id, endptr+1);
		if ( ! NT_STATUS_IS_OK(ret)) {
			/* if not valid form delete the entry */
			tdb_delete_bystring(cache->tdb, idkey);
			ret = NT_STATUS_NONE_MAPPED;
			goto done;
		}

		/* here ret == NT_STATUS_OK and id->mapped = ID_MAPPED */

		if (t <= now) {
			/* If we've been told to be offline - stay in
			   that state... */
			if ( IS_DOMAIN_OFFLINE(our_domain) ) {
				DEBUG(10,("idmap_cache_map_sid: idmap is offline\n"));
				goto done;
			}

			/* We're expired, set an error code
			   for upper layer */
			ret = NT_STATUS_SYNCHRONIZATION_REQUIRED;
		}

		goto done;
	}

	/* Was a negative cache hit */

	/* Ignore the negative cache when offline */

	if ( IS_DOMAIN_OFFLINE(our_domain) ) {
		DEBUG(10,("idmap_cache_map_sid: idmap is offline\n"));
		ret = NT_STATUS_NONE_MAPPED;

		goto done;
	}

	/* Process the negative cache hit */

		if (t <= now) {
		/* We're expired.  Return not mapped */
			ret = NT_STATUS_NONE_MAPPED;
		} else {
		/* this is not mapped is it was a negative cache hit */
			id->status = ID_UNMAPPED;
			ret = NT_STATUS_OK;
		}

done:
	SAFE_FREE(databuf.dptr);
	talloc_free(idkey);
	return ret;
}

