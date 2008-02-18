/* 
   Unix SMB/CIFS implementation.
   Network neighbourhood browser.
   
   Copyright (C) Tim Potter      2000
   Copyright (C) Jelmer Vernooij 2003
   
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

static int use_bcast;

/* How low can we go? */

enum tree_level {LEV_WORKGROUP, LEV_SERVER, LEV_SHARE};
static enum tree_level level = LEV_SHARE;

/* Holds a list of workgroups or servers */

struct name_list {
        struct name_list *prev, *next;
        char *name, *comment;
        uint32 server_type;
};

static struct name_list *workgroups, *servers, *shares;

static void free_name_list(struct name_list *list)
{
        while(list)
                DLIST_REMOVE(list, list);
}

static void add_name(const char *machine_name, uint32 server_type,
                     const char *comment, void *state)
{
        struct name_list **name_list = (struct name_list **)state;
        struct name_list *new_name;

        new_name = SMB_MALLOC_P(struct name_list);

        if (!new_name)
                return;

        ZERO_STRUCTP(new_name);

	new_name->name = SMB_STRDUP(machine_name);
	new_name->comment = SMB_STRDUP(comment);
        new_name->server_type = server_type;

	if (!new_name->name || !new_name->comment) {
		SAFE_FREE(new_name->name);
		SAFE_FREE(new_name->comment);
		SAFE_FREE(new_name);
		return;
	}

        DLIST_ADD(*name_list, new_name);
}

/****************************************************************************
  display tree of smb workgroups, servers and shares
****************************************************************************/
static bool get_workgroups(struct user_auth_info *user_info)
{
        struct cli_state *cli;
        struct sockaddr_storage server_ss;
	TALLOC_CTX *ctx = talloc_tos();
	char *master_workgroup = NULL;

        /* Try to connect to a #1d name of our current workgroup.  If that
           doesn't work broadcast for a master browser and then jump off
           that workgroup. */

	master_workgroup = talloc_strdup(ctx, lp_workgroup());
	if (!master_workgroup) {
		return false;
	}

        if (!use_bcast && !find_master_ip(lp_workgroup(), &server_ss)) {
                DEBUG(4, ("Unable to find master browser for workgroup %s, falling back to broadcast\n", 
			  master_workgroup));
				use_bcast = True;
		} else if(!use_bcast) {
			char addr[INET6_ADDRSTRLEN];
			print_sockaddr(addr, sizeof(addr), &server_ss);
			if (!(cli = get_ipc_connect(addr, &server_ss, user_info)))
				return False;
		}

		if (!(cli = get_ipc_connect_master_ip_bcast(talloc_tos(),
							user_info,
							&master_workgroup))) {
			DEBUG(4, ("Unable to find master browser by "
				  "broadcast\n"));
			return False;
        }

        if (!cli_NetServerEnum(cli, master_workgroup,
                               SV_TYPE_DOMAIN_ENUM, add_name, &workgroups))
                return False;

        return True;
}

/* Retrieve the list of servers for a given workgroup */

static bool get_servers(char *workgroup, struct user_auth_info *user_info)
{
        struct cli_state *cli;
        struct sockaddr_storage server_ss;
	char addr[INET6_ADDRSTRLEN];

        /* Open an IPC$ connection to the master browser for the workgroup */

        if (!find_master_ip(workgroup, &server_ss)) {
                DEBUG(4, ("Cannot find master browser for workgroup %s\n",
                          workgroup));
                return False;
        }

	print_sockaddr(addr, sizeof(addr), &server_ss);
        if (!(cli = get_ipc_connect(addr, &server_ss, user_info)))
                return False;

        if (!cli_NetServerEnum(cli, workgroup, SV_TYPE_ALL, add_name,
                               &servers))
                return False;

        return True;
}

static bool get_rpc_shares(struct cli_state *cli,
			   void (*fn)(const char *, uint32, const char *, void *),
			   void *state)
{
	NTSTATUS status;
	struct rpc_pipe_client *pipe_hnd;
	TALLOC_CTX *mem_ctx;
	ENUM_HND enum_hnd;
	WERROR werr;
	SRV_SHARE_INFO_CTR ctr;
	int i;

	mem_ctx = talloc_new(NULL);
	if (mem_ctx == NULL) {
		DEBUG(0, ("talloc_new failed\n"));
		return False;
	}

	init_enum_hnd(&enum_hnd, 0);

	pipe_hnd = cli_rpc_pipe_open_noauth(cli, PI_SRVSVC, &status);

	if (pipe_hnd == NULL) {
		DEBUG(10, ("Could not connect to srvsvc pipe: %s\n",
			   nt_errstr(status)));
		TALLOC_FREE(mem_ctx);
		return False;
	}

	werr = rpccli_srvsvc_net_share_enum(pipe_hnd, mem_ctx, 1, &ctr,
					    0xffffffff, &enum_hnd);

	if (!W_ERROR_IS_OK(werr)) {
		TALLOC_FREE(mem_ctx);
		cli_rpc_pipe_close(pipe_hnd);
		return False;
	}

	for (i=0; i<ctr.num_entries; i++) {
		SRV_SHARE_INFO_1 *info = &ctr.share.info1[i];
		char *name, *comment;
		name = rpcstr_pull_unistr2_talloc(
			mem_ctx, &info->info_1_str.uni_netname);
		comment = rpcstr_pull_unistr2_talloc(
			mem_ctx, &info->info_1_str.uni_remark);
		fn(name, info->info_1.type, comment, state);
	}

	TALLOC_FREE(mem_ctx);
	cli_rpc_pipe_close(pipe_hnd);
	return True;
}


static bool get_shares(char *server_name, struct user_auth_info *user_info)
{
        struct cli_state *cli;

        if (!(cli = get_ipc_connect(server_name, NULL, user_info)))
                return False;

	if (get_rpc_shares(cli, add_name, &shares))
		return True;

        if (!cli_RNetShareEnum(cli, add_name, &shares))
                return False;

        return True;
}

static bool print_tree(struct user_auth_info *user_info)
{
        struct name_list *wg, *sv, *sh;

        /* List workgroups */

        if (!get_workgroups(user_info))
                return False;

        for (wg = workgroups; wg; wg = wg->next) {

                printf("%s\n", wg->name);

                /* List servers */

                free_name_list(servers);
                servers = NULL;

                if (level == LEV_WORKGROUP || 
                    !get_servers(wg->name, user_info))
                        continue;

                for (sv = servers; sv; sv = sv->next) {

                        printf("\t\\\\%-15s\t\t%s\n", 
			       sv->name, sv->comment);

                        /* List shares */

                        free_name_list(shares);
                        shares = NULL;

                        if (level == LEV_SERVER ||
                            !get_shares(sv->name, user_info))
                                continue;

                        for (sh = shares; sh; sh = sh->next) {
                                printf("\t\t\\\\%s\\%-15s\t%s\n", 
				       sv->name, sh->name, sh->comment);
                        }
                }
        }

        return True;
}

/****************************************************************************
  main program
****************************************************************************/
 int main(int argc,char *argv[])
{
	TALLOC_CTX *frame = talloc_stackframe();
	struct user_auth_info local_auth_info;
	struct poptOption long_options[] = {
		POPT_AUTOHELP
		{ "broadcast", 'b', POPT_ARG_VAL, &use_bcast, True, "Use broadcast instead of using the master browser" },
		{ "domains", 'D', POPT_ARG_VAL, &level, LEV_WORKGROUP, "List only domains (workgroups) of tree" },
		{ "servers", 'S', POPT_ARG_VAL, &level, LEV_SERVER, "List domains(workgroups) and servers of tree" },
		POPT_COMMON_SAMBA
		POPT_COMMON_CREDENTIALS
		POPT_TABLEEND
	};
	poptContext pc;

	/* Initialise samba stuff */
	load_case_tables();

	setlinebuf(stdout);

	dbf = x_stderr;

	setup_logging(argv[0],True);

	pc = poptGetContext("smbtree", argc, (const char **)argv, long_options,
						POPT_CONTEXT_KEEP_FIRST);
	while(poptGetNextOpt(pc) != -1);
	poptFreeContext(pc);

	lp_load(get_dyn_CONFIGFILE(),True,False,False,True);
	load_interfaces();

	/* Parse command line args */

	if (!get_cmdline_auth_info_got_pass()) {
		char *pass = getpass("Password: ");
		if (pass) {
			set_cmdline_auth_info_password(pass);
		}
	}

	/* Now do our stuff */

	if (!get_cmdline_auth_info_copy(&local_auth_info)) {
		return 1;
	}

        if (!print_tree(&local_auth_info)) {
		TALLOC_FREE(frame);
                return 1;
	}

	TALLOC_FREE(frame);
	return 0;
}
