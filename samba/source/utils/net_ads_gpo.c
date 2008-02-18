/* 
   Samba Unix/Linux SMB client library 
   net ads commands for Group Policy
   Copyright (C) 2005 Guenther Deschner (gd@samba.org)

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
#include "utils/net.h"

#ifdef HAVE_ADS

static int net_ads_gpo_usage(int argc, const char **argv)
{
	d_printf(
		"%s ads gpo <COMMAND>\n"\
"<COMMAND> can be either:\n"\
"  APPLY        Apply GPOs for machine/user\n"\
"  GETGPO       Lists specified GPO\n"\
"  HELP         Prints this help message\n"\
"  LINKADD      Link a container to a GPO\n"\
/* "  LINKDELETE   Delete a gPLink from a container\n"\ */
"  LINKGET      Lists gPLink of a containter\n"\
"  LIST         Lists all GPOs for machine/user\n"\
"  LISTALL      Lists all GPOs on a DC\n"\
"  REFRESH      Lists all GPOs assigned to an account and downloads them\n"\
"\n",
		NET_PROGRAM_NAME);
	return -1;
}

static int net_ads_gpo_refresh(int argc, const char **argv)
{
	TALLOC_CTX *mem_ctx;
	ADS_STRUCT *ads;
	ADS_STATUS status;
	const char *dn = NULL;
	struct GROUP_POLICY_OBJECT *gpo_list = NULL;
	uint32 uac = 0;
	uint32 flags = 0;
	struct GROUP_POLICY_OBJECT *gpo;
	NTSTATUS result;
	struct nt_user_token *token = NULL;

	if (argc < 1) {
		printf("usage: %s ads gpo refresh <username|machinename>\n",
		       NET_PROGRAM_NAME);
		return -1;
	}

	mem_ctx = talloc_init("net_ads_gpo_refresh");
	if (mem_ctx == NULL) {
		return -1;
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_find_samaccount(ads, mem_ctx, argv[0], &uac, &dn);
	if (!ADS_ERR_OK(status)) {
		printf("failed to find samaccount for %s\n", argv[0]);
		goto out;
	}

	if (uac & UF_WORKSTATION_TRUST_ACCOUNT) {
		flags |= GPO_LIST_FLAG_MACHINE;
	}

	printf("\n%s: '%s' has dn: '%s'\n\n", 
		(uac & UF_WORKSTATION_TRUST_ACCOUNT) ? "machine" : "user", 
		argv[0], dn);

	status = ads_get_sid_token(ads, mem_ctx, dn, &token);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_get_gpo_list(ads, mem_ctx, dn, flags, token, &gpo_list);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	if (!NT_STATUS_IS_OK(result = check_refresh_gpo_list(ads, mem_ctx, flags, gpo_list))) {
		printf("failed to refresh GPOs: %s\n", nt_errstr(result));
		goto out;
	}

	for (gpo = gpo_list; gpo; gpo = gpo->next) {

		char *server, *share, *nt_path, *unix_path;

		printf("--------------------------------------\n");
		printf("Name:\t\t\t%s\n", gpo->display_name);
		printf("LDAP GPO version:\t%d (user: %d, machine: %d)\n",
			gpo->version,
			GPO_VERSION_USER(gpo->version),
			GPO_VERSION_MACHINE(gpo->version));

		result = gpo_explode_filesyspath(mem_ctx, gpo->file_sys_path,
						 &server, &share, &nt_path, &unix_path);
		if (!NT_STATUS_IS_OK(result)) {
			printf("got: %s\n", nt_errstr(result));
		}

		printf("GPO stored on server: %s, share: %s\n", server, share);
		printf("\tremote path:\t%s\n", nt_path);
		printf("\tlocal path:\t%s\n", unix_path);
	}

 out:
	ads_destroy(&ads);
	talloc_destroy(mem_ctx);
	return 0;
}

static int net_ads_gpo_list_all(int argc, const char **argv)
{
	ADS_STRUCT *ads;
	ADS_STATUS status;
	LDAPMessage *res = NULL;
	int num_reply = 0;
	LDAPMessage *msg = NULL;
	struct GROUP_POLICY_OBJECT gpo;
	TALLOC_CTX *mem_ctx;
	char *dn;
	const char *attrs[] = {
		"versionNumber",
		"flags",
		"gPCFileSysPath",
		"displayName",
		"name",
		"gPCMachineExtensionNames",
		"gPCUserExtensionNames",
		"ntSecurityDescriptor",
		NULL
	};

	mem_ctx = talloc_init("net_ads_gpo_list_all");
	if (mem_ctx == NULL) {
		return -1;
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_do_search_all_sd_flags(ads, ads->config.bind_path,
					    LDAP_SCOPE_SUBTREE,
					    "(objectclass=groupPolicyContainer)",
					    attrs,
					    DACL_SECURITY_INFORMATION,
					    &res);

	if (!ADS_ERR_OK(status)) {
		d_printf("search failed: %s\n", ads_errstr(status));
		goto out;
	}	

	num_reply = ads_count_replies(ads, res);
	
	d_printf("Got %d replies\n\n", num_reply);

	/* dump the results */
	for (msg = ads_first_entry(ads, res); msg; msg = ads_next_entry(ads, msg)) {

		if ((dn = ads_get_dn(ads, msg)) == NULL) {
			goto out;
		}

		status = ads_parse_gpo(ads, mem_ctx, msg, dn, &gpo);

		if (!ADS_ERR_OK(status)) {
			d_printf("ads_parse_gpo failed: %s\n", ads_errstr(status));
			ads_memfree(ads, dn);
			goto out;
		}	

		dump_gpo(ads, mem_ctx, &gpo, 0);
		ads_memfree(ads, dn);
	}

out:
	ads_msgfree(ads, res);

	talloc_destroy(mem_ctx);
	ads_destroy(&ads);
	
	return 0;
}

static int net_ads_gpo_list(int argc, const char **argv)
{
	ADS_STRUCT *ads;
	ADS_STATUS status;
	LDAPMessage *res = NULL;
	TALLOC_CTX *mem_ctx;
	const char *dn = NULL;
	uint32 uac = 0;
	uint32 flags = 0;
	struct GROUP_POLICY_OBJECT *gpo_list;
	struct nt_user_token *token = NULL;

	if (argc < 1) {
		printf("usage: %s ads gpo list <username|machinename>\n",
		       NET_PROGRAM_NAME);
		return -1;
	}

	mem_ctx = talloc_init("net_ads_gpo_list");
	if (mem_ctx == NULL) {
		goto out;
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_find_samaccount(ads, mem_ctx, argv[0], &uac, &dn);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	if (uac & UF_WORKSTATION_TRUST_ACCOUNT) {
		flags |= GPO_LIST_FLAG_MACHINE;
	}

	printf("%s: '%s' has dn: '%s'\n", 
		(uac & UF_WORKSTATION_TRUST_ACCOUNT) ? "machine" : "user", 
		argv[0], dn);

	status = ads_get_sid_token(ads, mem_ctx, dn, &token);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_get_gpo_list(ads, mem_ctx, dn, flags, token, &gpo_list);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	dump_gpo_list(ads, mem_ctx, gpo_list, 0);

out:
	ads_msgfree(ads, res);

	talloc_destroy(mem_ctx);
	ads_destroy(&ads);
	
	return 0;
}

#if 0 /* not yet */
static int net_ads_gpo_apply(int argc, const char **argv)
{
	TALLOC_CTX *mem_ctx;
	ADS_STRUCT *ads;
	ADS_STATUS status;
	const char *dn = NULL;
	struct GROUP_POLICY_OBJECT *gpo_list;
	uint32 uac = 0;
	uint32 flags = 0;
	
	if (argc < 1) {
		printf("usage: %s ads gpo apply <username|machinename>\n",
		       NET_PROGRAM_NAME);
		return -1;
	}

	mem_ctx = talloc_init("net_ads_gpo_apply");
	if (mem_ctx == NULL) {
		goto out;
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_find_samaccount(ads, mem_ctx, argv[0], &uac, &dn);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	if (uac & UF_WORKSTATION_TRUST_ACCOUNT) {
		flags |= GPO_LIST_FLAG_MACHINE;
	}

	printf("%s: '%s' has dn: '%s'\n", 
		(uac & UF_WORKSTATION_TRUST_ACCOUNT) ? "machine" : "user", 
		argv[0], dn);

	status = ads_get_gpo_list(ads, mem_ctx, dn, flags, &gpo_list);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	/* FIXME: allow to process just a single extension */
	status = gpo_process_gpo_list(ads, mem_ctx, gpo_list, NULL, flags); 
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

out:
	ads_destroy(&ads);
	talloc_destroy(mem_ctx);
	return 0;
}

#endif

static int net_ads_gpo_link_get(int argc, const char **argv)
{
	ADS_STRUCT *ads;
	ADS_STATUS status;
	TALLOC_CTX *mem_ctx;
	struct GP_LINK gp_link;

	if (argc < 1) {
		printf("usage: %s ads gpo linkget <linkname>\n",
		       NET_PROGRAM_NAME);
		return -1;
	}

	mem_ctx = talloc_init("add_gpo_link");
	if (mem_ctx == NULL) {
		return -1;
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_get_gpo_link(ads, mem_ctx, argv[0], &gp_link);
	if (!ADS_ERR_OK(status)) {
		d_printf("get link for %s failed: %s\n", argv[0], ads_errstr(status));
		goto out;
	}	

	dump_gplink(ads, mem_ctx, &gp_link);

out:
	talloc_destroy(mem_ctx);
	ads_destroy(&ads);

	return 0;
}

static int net_ads_gpo_link_add(int argc, const char **argv)
{
	ADS_STRUCT *ads;
	ADS_STATUS status;
	uint32 gpo_opt = 0;
	TALLOC_CTX *mem_ctx;

	if (argc < 2) {
		printf("usage: %s ads gpo linkadd <linkdn> <gpodn> [options]\n",
		       NET_PROGRAM_NAME);
		printf("note: DNs must be provided properly escaped.\n      See RFC 4514 for details\n");
		return -1;
	}

	mem_ctx = talloc_init("add_gpo_link");
	if (mem_ctx == NULL) {
		return -1;
	}

	if (argc == 3) {
		gpo_opt = atoi(argv[2]);
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_add_gpo_link(ads, mem_ctx, argv[0], argv[1], gpo_opt);
	if (!ADS_ERR_OK(status)) {
		d_printf("link add failed: %s\n", ads_errstr(status));
		goto out;
	}

out:
	talloc_destroy(mem_ctx);
	ads_destroy(&ads);

	return 0;
}

#if 0 /* broken */

static int net_ads_gpo_link_delete(int argc, const char **argv)
{
	ADS_STRUCT *ads;
	ADS_STATUS status;
	TALLOC_CTX *mem_ctx;

	if (argc < 2) {
		return -1;
	}

	mem_ctx = talloc_init("delete_gpo_link");
	if (mem_ctx == NULL) {
		return -1;
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	status = ads_delete_gpo_link(ads, mem_ctx, argv[0], argv[1]);
	if (!ADS_ERR_OK(status)) {
		d_printf("delete link failed: %s\n", ads_errstr(status));
		goto out;
	}	

out:
	talloc_destroy(mem_ctx);
	ads_destroy(&ads);

	return 0;
}

#endif

static int net_ads_gpo_get_gpo(int argc, const char **argv)
{
	ADS_STRUCT *ads;
	ADS_STATUS status;
	TALLOC_CTX *mem_ctx;
	struct GROUP_POLICY_OBJECT gpo;

	if (argc < 1) {
		printf("usage: %s ads gpo getgpo <gpo>\n",
		       NET_PROGRAM_NAME);
		return -1;
	}

	mem_ctx = talloc_init("add_gpo_get_gpo");
	if (mem_ctx == NULL) {
		return -1;
	}

	status = ads_startup(False, &ads);
	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	if (strnequal(argv[0], "CN={", strlen("CN={"))) {
		status = ads_get_gpo(ads, mem_ctx, argv[0], NULL, NULL, &gpo);
	} else {
		status = ads_get_gpo(ads, mem_ctx, NULL, argv[0], NULL, &gpo);
	}

	if (!ADS_ERR_OK(status)) {
		d_printf("get gpo for [%s] failed: %s\n", argv[0], ads_errstr(status));
		goto out;
	}	

	dump_gpo(ads, mem_ctx, &gpo, 1);

out:
	talloc_destroy(mem_ctx);
	ads_destroy(&ads);

	return 0;
}

int net_ads_gpo(int argc, const char **argv)
{
	struct functable func[] = {
		/* {"APPLY", net_ads_gpo_apply}, */
		{"GETGPO", net_ads_gpo_get_gpo},
		{"HELP", net_ads_gpo_usage},
		{"LINKADD", net_ads_gpo_link_add},
		/* {"LINKDELETE", net_ads_gpo_link_delete}, */
		{"LINKGET", net_ads_gpo_link_get},
		{"LIST", net_ads_gpo_list},
		{"LISTALL", net_ads_gpo_list_all},
		{"REFRESH", net_ads_gpo_refresh},
		{NULL, NULL}
	};

	return net_run_function(argc, argv, func, net_ads_gpo_usage);
}

#endif /* HAVE_ADS */
