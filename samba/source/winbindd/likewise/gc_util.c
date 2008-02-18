/*
 * idmap_lwidentity: Global Catalog search interface
 *
 * Copyright (C) Gerald (Jerry) Carter 2007
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "includes.h"
#include "idmap_lwidentity.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_IDMAP

static struct gc_info *_gc_server_list = NULL;


/**********************************************************************
 *********************************************************************/

static struct gc_info *gc_list_head(void)
{
	return _gc_server_list;
}

/**********************************************************************
 Checks if either of the domains is a subdomain of the other
 *********************************************************************/

static bool is_subdomain(const char* a, const char *b)
{
	char *s;
	TALLOC_CTX *frame = talloc_stackframe();
	char *x, *y;
	bool ret = false;

	/* Trivial cases */

	if (!a && !b)
		return true;

	if (!a || !b)
		return false;

	/* Normalize the case */

	x = talloc_strdup(frame, a);
	y = talloc_strdup(frame, b);
	if (!x || !y) {
		ret = false;
		goto done;
	}

	strupper_m(x);
	strupper_m(y);

	/* Exact match */

	if (strcmp(x, y) == 0) {
		ret = true;
		goto done;
	}

	/* Check for trailing substrings */

	s = strstr_m(x, y);
	if (s && (strlen(s) == strlen(y))) {
		ret = true;
		goto done;
	}

	s = strstr_m(y, x);
	if (s && (strlen(s) == strlen(x))) {
		ret = true;
		goto done;
	}

done:
	talloc_destroy(frame);

	return ret;
}


/**********************************************************************
 *********************************************************************/

static NTSTATUS gc_add_forest(const char *forest)
{
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	struct gc_info *gc = NULL;
	char *dn;
	ADS_STRUCT *ads = NULL;
	struct likewise_cell *primary_cell = NULL;

	primary_cell = cell_list_head();
	if (!primary_cell) {
		nt_status = NT_STATUS_INVALID_SERVER_STATE;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	/* Check for duplicates */

	gc = gc_list_head();
	while (gc) {
		if (strequal (gc->forest_name, forest))
			break;
		gc = gc->next;
	}

	if (gc) {
		DEBUG(10,("gc_add_forest: %s already in list\n", forest));
		return NT_STATUS_OK;
	}

	if ((gc = TALLOC_ZERO_P(NULL, struct gc_info)) == NULL) {
		nt_status = NT_STATUS_NO_MEMORY;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	gc->forest_name = talloc_strdup(gc, forest);
	BAIL_ON_PTR_ERROR(gc->forest_name, nt_status);

	dn = ads_build_dn(gc->forest_name);
	BAIL_ON_PTR_ERROR(dn, nt_status);

	gc->search_base = talloc_strdup(gc, dn);
	SAFE_FREE(dn);
	BAIL_ON_PTR_ERROR(gc->search_base, nt_status);

	/* Connect to the cell to find the forest settings.  This must
	   be done before we mark the cell as a GC cell connection to
	   get the correct information. */

	nt_status = cell_connect_dn(&gc->forest_cell, gc->search_base);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

	/* It is possible to belong to a non-forest cell and a
	   non-provisioned forest (at our domain levele). In that
	   case, we should just inherit the flags from our primary
	   cell since the GC searches will match our own schema
	   model. */

	if (is_subdomain(primary_cell->dns_domain, gc->forest_name)) {
		cell_set_flags(gc->forest_cell, cell_flags(primary_cell));
	} else {
		/* outside of our domain */
		nt_status = cell_lookup_settings(gc->forest_cell);
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	/* Drop the connection now that we have the settings */

	ads = cell_connection(gc->forest_cell);
	ads_destroy(&ads);
	cell_set_connection(gc->forest_cell, NULL);

	/* Set a couple of necessary flags to mark this for
	   cell_do_search() */

	cell_set_flags(gc->forest_cell, LWCELL_FLAG_GC_CELL);

	DLIST_ADD_END(_gc_server_list, gc, struct gc_info*);

	nt_status = NT_STATUS_OK;

done:
	if (!NT_STATUS_IS_OK(nt_status)) {
		talloc_destroy(gc);
		DEBUG(0,("LWI: Failed to add new GC connection for %s (%s)\n",
			 forest, nt_errstr(nt_status)));
	}

	return nt_status;
}

/**********************************************************************
 *********************************************************************/

static void gc_server_list_destroy(void)
{
	struct gc_info *gc = gc_list_head();

	while (gc) {
		struct gc_info *p = gc->next;

		cell_destroy(gc->forest_cell);
		talloc_destroy(gc);

		gc = p;
	}

	_gc_server_list = NULL;

	return;
}

/**********************************************************************
 Setup the initial list of forests and initial the forest cell
 settings for each.  FIXME!!!
 *********************************************************************/

 NTSTATUS gc_init_list(void)
{
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	struct winbindd_tdc_domain *domains = NULL;
	size_t num_domains = 0;
	int i;

	if (_gc_server_list != NULL) {
		gc_server_list_destroy();
	}

	if (!wcache_tdc_fetch_list(&domains, &num_domains)) {
		nt_status = NT_STATUS_CANT_ACCESS_DOMAIN_INFO;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	/* Find our forst first */

	for (i=0; i<num_domains; i++) {
		uint32_t flags = (NETR_TRUST_FLAG_TREEROOT|NETR_TRUST_FLAG_IN_FOREST);

		if ((domains[i].trust_flags & flags) == flags) {
			nt_status = gc_add_forest(domains[i].dns_name);
			BAIL_ON_NTSTATUS_ERROR(nt_status);

			break;
		}
	}

	/* Now add trusted forests.  gc_add_forest() will filter out
	   duplicates */

	for (i=0; i<num_domains; i++) {
		uint32_t flags = domains[i].trust_flags;
		uint32_t attribs = domains[i].trust_attribs;

		if ((flags & NETR_TRUST_FLAG_INBOUND)
		    && (attribs & NETR_TRUST_ATTRIBUTE_FOREST_TRANSITIVE))
		{
			nt_status = gc_add_forest(domains[i].dns_name);
			WARN_ON_NTSTATUS_ERROR(nt_status);
		}
	}

	nt_status = NT_STATUS_OK;

done:
	if (!NT_STATUS_IS_OK(nt_status)) {
		DEBUG(2,("LWI: Failed to initialized GC list (%s)\n",
			 nt_errstr(nt_status)));
	}

	TALLOC_FREE(domains);

	return nt_status;
}


/**********************************************************************
 *********************************************************************/

 struct gc_info *gc_search_start(void)
{
	NTSTATUS nt_status = NT_STATUS_OK;
	struct gc_info *gc = gc_list_head();

	if (!gc) {
		nt_status = gc_init_list();
		BAIL_ON_NTSTATUS_ERROR(nt_status);

		gc = gc_list_head();
	}

done:
	if (!NT_STATUS_IS_OK(nt_status)) {
		DEBUG(2,("LWI: Failed to initialize GC list (%s)\n",
			 nt_errstr(nt_status)));
	}

	return gc;
}

/**********************************************************************
 Search Global Catalog.  Always search our own forest.  The flags set
 controls whether or not we search cross forest.  Assume that the
 resulting set is always returned from one GC so that we don't have to
 both combining the LDAPMessage * results
 *********************************************************************/

 NTSTATUS gc_search_forest(struct gc_info *gc,
			   LDAPMessage **msg,
			   const char *filter)
{
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	ADS_STATUS ads_status = ADS_ERROR_NT(NT_STATUS_UNSUCCESSFUL);
	const char *attrs[] = {"*", NULL};
	LDAPMessage *m = NULL;

	if (!gc || !msg || !filter) {
		nt_status = NT_STATUS_INVALID_PARAMETER;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	ads_status = cell_do_search(gc->forest_cell, gc->search_base,
				   LDAP_SCOPE_SUBTREE, filter, attrs, &m);
	nt_status = ads_ntstatus(ads_status);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

	*msg = m;

done:
	if (!NT_STATUS_IS_OK(nt_status)) {
		DEBUG(2,("LWI: Forest wide search %s failed (%s)\n",
			 filter, nt_errstr(nt_status)));
	}

	return nt_status;
}

/**********************************************************************
 Search all forests via GC and return the results in an array of
 ADS_STRUCT/LDAPMessage pairs.
 *********************************************************************/

 NTSTATUS gc_search_all_forests(const char *filter,
				ADS_STRUCT ***ads_list,
				LDAPMessage ***msg_list,
				int *num_resp, uint32_t flags)
{
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	struct gc_info *gc = NULL;
	uint32_t test_flags = LW_GC_SEARCH_CHECK_UNIQUE;

	*ads_list = NULL;
	*msg_list = NULL;
	*num_resp = 0;

	if ((gc = gc_search_start()) == NULL) {
		nt_status = NT_STATUS_INVALID_DOMAIN_STATE;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	while (gc) {
		LDAPMessage *m = NULL;

		nt_status = gc_search_forest(gc, &m, filter);
		if (!NT_STATUS_IS_OK(nt_status)) {
			gc = gc->next;
			continue;
		}

		nt_status = add_ads_result_to_array(cell_connection(gc->forest_cell),
						    m, ads_list, msg_list,
						    num_resp);
		BAIL_ON_NTSTATUS_ERROR(nt_status);

		/* If there can only be one match, then we are done */

		if ((*num_resp > 0) && ((flags & test_flags) == test_flags)) {
			break;
		}

		gc = gc->next;
	}

	if (*num_resp == 0) {
		nt_status = NT_STATUS_OBJECT_NAME_NOT_FOUND;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	nt_status = NT_STATUS_OK;

done:
	return nt_status;
}

/**********************************************************************
 Search all forests via GC and return the results in an array of
 ADS_STRUCT/LDAPMessage pairs.
 *********************************************************************/

 NTSTATUS gc_search_all_forests_unique(const char *filter,
				       ADS_STRUCT **ads,
				       LDAPMessage **msg)
{
	ADS_STRUCT **ads_list = NULL;
	LDAPMessage **msg_list = NULL;
	int num_resp;
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;

	nt_status = gc_search_all_forests(filter, &ads_list,
					  &msg_list, &num_resp,
					  LW_GC_SEARCH_CHECK_UNIQUE);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

	nt_status = check_result_unique(ads_list[0], msg_list[0]);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

	*ads = ads_list[0];
	*msg = msg_list[0];

done:
	/* Be care that we don't free the msg result being returned */

	if (!NT_STATUS_IS_OK(nt_status)) {
		free_result_array(ads_list, msg_list, num_resp);
	} else {
		talloc_destroy(ads_list);
		talloc_destroy(msg_list);
	}

	return nt_status;
}

/*********************************************************************
 ********************************************************************/

 NTSTATUS gc_name_to_sid(const char *domain,
			 const char *name,
			 DOM_SID *sid,
			 enum lsa_SidType *sid_type)
{
	TALLOC_CTX *frame = talloc_stackframe();
	char *p, *name_user;
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	char *name_filter;
	ADS_STRUCT *ads = NULL;
	LDAPMessage *msg = NULL;
	LDAPMessage *e = NULL;
	char *dn = NULL;
	char *dns_domain = NULL;
	ADS_STRUCT **ads_list = NULL;
	LDAPMessage **msg_list = NULL;
	int num_resp = 0;
	int i;

	/* Strip the "DOMAIN\" prefix if necessary and search for
	   a matching sAMAccountName in the forest */

	if ((p = strchr_m( name, '\\' )) == NULL)
		name_user = talloc_strdup( frame, name );
	else
		name_user = talloc_strdup( frame, p+1 );
	BAIL_ON_PTR_ERROR(name_user, nt_status);

	name_filter = talloc_asprintf(frame, "(sAMAccountName=%s)", name_user);
	BAIL_ON_PTR_ERROR(name_filter, nt_status);

	nt_status = gc_search_all_forests(name_filter, &ads_list,
					  &msg_list, &num_resp, 0);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

	/* Assume failure until we know otherwise*/

	nt_status = NT_STATUS_OBJECT_NAME_NOT_FOUND;

	/* Match the domain name from the DN */

	for (i=0; i<num_resp; i++) {
		ads = ads_list[i];
		msg = msg_list[i];

		e = ads_first_entry(ads, msg);
		while (e) {
			struct winbindd_tdc_domain *domain_rec;

			dn = ads_get_dn(ads, e);
			BAIL_ON_PTR_ERROR(dn, nt_status);

			dns_domain = cell_dn_to_dns(dn);
			SAFE_FREE(dn);
			BAIL_ON_PTR_ERROR(dns_domain, nt_status);

			domain_rec = wcache_tdc_fetch_domain(frame, dns_domain);
			SAFE_FREE(dns_domain);

			/* Ignore failures and continue the search */

			if (!domain_rec) {
				continue;
			}

			/* Check for a match on the domain name */

			if (strequal(domain, domain_rec->domain_name)) {
				if (!ads_pull_sid(ads, e, "objectSid", sid)) {
					nt_status = NT_STATUS_INVALID_SID;
					BAIL_ON_NTSTATUS_ERROR(nt_status);
				}

				talloc_destroy(domain_rec);

				nt_status = get_sid_type(ads, msg, sid_type);
				BAIL_ON_NTSTATUS_ERROR(nt_status);

				/* We're done! */
				nt_status = NT_STATUS_OK;
				break;
			}

			/* once more around thew merry-go-round */

			talloc_destroy(domain_rec);
			e = ads_next_entry(ads, e);
		}
	}

done:
	free_result_array(ads_list, msg_list, num_resp);
	talloc_destroy(frame);

	return nt_status;
}

/********************************************************************
 Pull an attribute string value
 *******************************************************************/

static NTSTATUS get_object_account_name(ADS_STRUCT *ads,
					LDAPMessage *msg,
					char **name)
{
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	char *sam_name = NULL;
	struct winbindd_tdc_domain *domain_rec = NULL;
	char *dns_domain = NULL;
	char *dn = NULL;
	TALLOC_CTX *frame = talloc_stackframe();
	int len;

	/* Check parameters */

	if (!ads || !msg || !name) {
		nt_status = NT_STATUS_INVALID_PARAMETER;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	/* get the name and domain */

	dn = ads_get_dn(ads, msg);
	BAIL_ON_PTR_ERROR(dn, nt_status);

	DEBUG(10,("get_object_account_name: dn = \"%s\"\n", dn));

	dns_domain = cell_dn_to_dns(dn);
	SAFE_FREE(dn);
	BAIL_ON_PTR_ERROR(dns_domain, nt_status);

	domain_rec = wcache_tdc_fetch_domain(frame, dns_domain);
	SAFE_FREE(dns_domain);

	if (!domain_rec) {
		nt_status = NT_STATUS_TRUSTED_DOMAIN_FAILURE;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	sam_name = ads_pull_string(ads, frame, msg, "sAMAccountName");
	BAIL_ON_PTR_ERROR(sam_name, nt_status);

	len = asprintf(name, "%s\\%s", domain_rec->domain_name, sam_name);
	if (len == -1) {
		*name = NULL;
		BAIL_ON_PTR_ERROR((*name), nt_status);
	}

	nt_status = NT_STATUS_OK;

done:
	talloc_destroy(frame);

	return nt_status;
}

/*********************************************************************
 ********************************************************************/

 NTSTATUS gc_sid_to_name(const DOM_SID *sid,
			 char **name,
			 enum lsa_SidType *sid_type)
{
	TALLOC_CTX *frame = talloc_stackframe();
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	char *filter;
	ADS_STRUCT *ads = NULL;
	LDAPMessage *msg = NULL;
	char *sid_string;

	*name = NULL;

	sid_string = sid_binstring(sid);
	BAIL_ON_PTR_ERROR(sid_string, nt_status);

	filter = talloc_asprintf(frame, "(objectSid=%s)", sid_string);
	SAFE_FREE(sid_string);
	BAIL_ON_PTR_ERROR(filter, nt_status);

	nt_status = gc_search_all_forests_unique(filter, &ads, &msg);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

	nt_status = get_object_account_name(ads, msg, name);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

	nt_status = get_sid_type(ads, msg, sid_type);
	BAIL_ON_NTSTATUS_ERROR(nt_status);

done:
	ads_msgfree(ads, msg);
	talloc_destroy(frame);

	return nt_status;
}

/**********************************************************************
 *********************************************************************/

 NTSTATUS add_ads_result_to_array(ADS_STRUCT *ads,
				  LDAPMessage *msg,
				  ADS_STRUCT ***ads_list,
				  LDAPMessage ***msg_list,
				  int *size)
{
	NTSTATUS nt_status = NT_STATUS_UNSUCCESSFUL;
	ADS_STRUCT **ads_tmp = NULL;
	LDAPMessage **msg_tmp = NULL;
	int count = *size;

	if (!ads || !msg) {
		nt_status = NT_STATUS_INVALID_PARAMETER;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

#if 0
	/* Don't add a response with no entries */

	if (ads_count_replies(ads, msg) == 0) {
		return NT_STATUS_OK;
	}
#endif

	if (count == 0) {
		ads_tmp = TALLOC_ARRAY(NULL, ADS_STRUCT*, 1);
		BAIL_ON_PTR_ERROR(ads_tmp, nt_status);

		msg_tmp = TALLOC_ARRAY(NULL, LDAPMessage*, 1);
		BAIL_ON_PTR_ERROR(msg_tmp, nt_status);
	} else {
		ads_tmp = TALLOC_REALLOC_ARRAY(*ads_list, *ads_list, ADS_STRUCT*,
					       count+1);
		BAIL_ON_PTR_ERROR(ads_tmp, nt_status);

		msg_tmp = TALLOC_REALLOC_ARRAY(*msg_list, *msg_list, LDAPMessage*,
					       count+1);
		BAIL_ON_PTR_ERROR(msg_tmp, nt_status);
	}

	ads_tmp[count] = ads;
	msg_tmp[count] = msg;
	count++;

	*ads_list = ads_tmp;
	*msg_list = msg_tmp;
	*size = count;

	nt_status = NT_STATUS_OK;

done:
	if (!NT_STATUS_IS_OK(nt_status)) {
		talloc_destroy(ads_tmp);
		talloc_destroy(msg_tmp);
	}

	return nt_status;
}

/**********************************************************************
 Frees search results.  Do not free the ads_list as these are
 references back to the GC search structures.
 *********************************************************************/

 void free_result_array(ADS_STRUCT **ads_list,
			LDAPMessage **msg_list,
			int num_resp)
{
	int i;

	for (i=0; i<num_resp; i++) {
		ads_msgfree(ads_list[i], msg_list[i]);
	}

	talloc_destroy(ads_list);
	talloc_destroy(msg_list);
}

/**********************************************************************
 Check that we have exactly one entry from the search
 *********************************************************************/

 NTSTATUS check_result_unique(ADS_STRUCT *ads, LDAPMessage *msg)
{
	NTSTATUS nt_status;
	int count;

	count = ads_count_replies(ads, msg);

	if (count <= 0) {
		nt_status = NT_STATUS_OBJECT_NAME_NOT_FOUND;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	if (count > 1) {
		nt_status = NT_STATUS_DUPLICATE_NAME;
		BAIL_ON_NTSTATUS_ERROR(nt_status);
	}

	nt_status = NT_STATUS_OK;

done:
	return nt_status;
}

