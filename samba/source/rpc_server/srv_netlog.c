/* 
 *  Unix SMB/CIFS implementation.
 *  RPC Pipe client / server routines
 *  Copyright (C) Andrew Tridgell              1992-1997,
 *  Copyright (C) Luke Kenneth Casson Leighton 1996-1997,
 *  Copyright (C) Paul Ashton                       1997,
 *  Copyright (C) Jeremy Allison               1998-2001,
 *  Copyright (C) Jim McDonough <jmcd@us.ibm.com>   2003.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

/* This is the interface to the netlogon pipe. */

#include "includes.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_RPC_SRV

/*************************************************************************
 api_net_req_chal:
 *************************************************************************/

static bool api_net_req_chal(pipes_struct *p)
{
	NET_Q_REQ_CHAL q_u;
	NET_R_REQ_CHAL r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	/* grab the challenge... */
	if(!net_io_q_req_chal("", &q_u, data, 0)) {
		DEBUG(0,("api_net_req_chal: Failed to unmarshall NET_Q_REQ_CHAL.\n"));
		return False;
	}

	r_u.status = _net_req_chal(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_req_chal("", &r_u, rdata, 0)) {
		DEBUG(0,("api_net_req_chal: Failed to marshall NET_R_REQ_CHAL.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_auth:
 *************************************************************************/

static bool api_net_auth(pipes_struct *p)
{
	NET_Q_AUTH q_u;
	NET_R_AUTH r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	/* grab the challenge... */
	if(!net_io_q_auth("", &q_u, data, 0)) {
		DEBUG(0,("api_net_auth: Failed to unmarshall NET_Q_AUTH.\n"));
		return False;
	}

	r_u.status = _net_auth(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_auth("", &r_u, rdata, 0)) {
		DEBUG(0,("api_net_auth: Failed to marshall NET_R_AUTH.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_auth_2:
 *************************************************************************/

static bool api_net_auth_2(pipes_struct *p)
{
	NET_Q_AUTH_2 q_u;
	NET_R_AUTH_2 r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	/* grab the challenge... */
	if(!net_io_q_auth_2("", &q_u, data, 0)) {
		DEBUG(0,("api_net_auth_2: Failed to unmarshall NET_Q_AUTH_2.\n"));
		return False;
	}

	r_u.status = _net_auth_2(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_auth_2("", &r_u, rdata, 0)) {
		DEBUG(0,("api_net_auth_2: Failed to marshall NET_R_AUTH_2.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_srv_pwset:
 *************************************************************************/

static bool api_net_srv_pwset(pipes_struct *p)
{
	NET_Q_SRV_PWSET q_u;
	NET_R_SRV_PWSET r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	/* grab the challenge and encrypted password ... */
	if(!net_io_q_srv_pwset("", &q_u, data, 0)) {
		DEBUG(0,("api_net_srv_pwset: Failed to unmarshall NET_Q_SRV_PWSET.\n"));
		return False;
	}

	r_u.status = _net_srv_pwset(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_srv_pwset("", &r_u, rdata, 0)) {
		DEBUG(0,("api_net_srv_pwset: Failed to marshall NET_R_SRV_PWSET.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_sam_logoff:
 *************************************************************************/

static bool api_net_sam_logoff(pipes_struct *p)
{
	NET_Q_SAM_LOGOFF q_u;
	NET_R_SAM_LOGOFF r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	if(!net_io_q_sam_logoff("", &q_u, data, 0)) {
		DEBUG(0,("api_net_sam_logoff: Failed to unmarshall NET_Q_SAM_LOGOFF.\n"));
		return False;
	}

	r_u.status = _net_sam_logoff(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_sam_logoff("", &r_u, rdata, 0)) {
		DEBUG(0,("api_net_sam_logoff: Failed to marshall NET_R_SAM_LOGOFF.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_sam_logon:
 *************************************************************************/

static bool api_net_sam_logon(pipes_struct *p)
{
	NET_Q_SAM_LOGON q_u;
	NET_R_SAM_LOGON r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;
    
	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	if(!net_io_q_sam_logon("", &q_u, data, 0)) {
		DEBUG(0, ("api_net_sam_logon: Failed to unmarshall NET_Q_SAM_LOGON.\n"));
		return False;
	}
   
	r_u.status = _net_sam_logon(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_sam_logon("", &r_u, rdata, 0)) {
		DEBUG(0,("api_net_sam_logon: Failed to marshall NET_R_SAM_LOGON.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_trust_dom_list:
 *************************************************************************/

static bool api_net_trust_dom_list(pipes_struct *p)
{
	NET_Q_TRUST_DOM_LIST q_u;
	NET_R_TRUST_DOM_LIST r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	/* grab the lsa trusted domain list query... */
	if(!net_io_q_trust_dom("", &q_u, data, 0)) {
		DEBUG(0,("api_net_trust_dom_list: Failed to unmarshall NET_Q_TRUST_DOM_LIST.\n"));
		return False;
	}

	/* construct reply. */
	r_u.status = _net_trust_dom_list(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_trust_dom("", &r_u, rdata, 0)) {
		DEBUG(0,("net_reply_trust_dom_list: Failed to marshall NET_R_TRUST_DOM_LIST.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_logon_ctrl2:
 *************************************************************************/

static bool api_net_logon_ctrl2(pipes_struct *p)
{
	NET_Q_LOGON_CTRL2 q_u;
	NET_R_LOGON_CTRL2 r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);


	/* grab the lsa netlogon ctrl2 query... */
	if(!net_io_q_logon_ctrl2("", &q_u, data, 0)) {
		DEBUG(0,("api_net_logon_ctrl2: Failed to unmarshall NET_Q_LOGON_CTRL2.\n"));
		return False;
	}

	r_u.status = _net_logon_ctrl2(p, &q_u, &r_u);

	if(!net_io_r_logon_ctrl2("", &r_u, rdata, 0)) {
		DEBUG(0,("net_reply_logon_ctrl2: Failed to marshall NET_R_LOGON_CTRL2.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_logon_ctrl:
 *************************************************************************/

static bool api_net_logon_ctrl(pipes_struct *p)
{
	NET_Q_LOGON_CTRL q_u;
	NET_R_LOGON_CTRL r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;

	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	/* grab the lsa netlogon ctrl query... */
	if(!net_io_q_logon_ctrl("", &q_u, data, 0)) {
		DEBUG(0,("api_net_logon_ctrl: Failed to unmarshall NET_Q_LOGON_CTRL.\n"));
		return False;
	}

	r_u.status = _net_logon_ctrl(p, &q_u, &r_u);

	if(!net_io_r_logon_ctrl("", &r_u, rdata, 0)) {
		DEBUG(0,("net_reply_logon_ctrl2: Failed to marshall NET_R_LOGON_CTRL.\n"));
		return False;
	}

	return True;
}

/*************************************************************************
 api_net_sam_logon_ex:
 *************************************************************************/

static bool api_net_sam_logon_ex(pipes_struct *p)
{
	NET_Q_SAM_LOGON_EX q_u;
	NET_R_SAM_LOGON_EX r_u;
	prs_struct *data = &p->in_data.data;
	prs_struct *rdata = &p->out_data.rdata;
    
	ZERO_STRUCT(q_u);
	ZERO_STRUCT(r_u);

	if(!net_io_q_sam_logon_ex("", &q_u, data, 0)) {
		DEBUG(0, ("api_net_sam_logon_ex: Failed to unmarshall NET_Q_SAM_LOGON_EX.\n"));
		return False;
	}
   
	r_u.status = _net_sam_logon_ex(p, &q_u, &r_u);

	/* store the response in the SMB stream */
	if(!net_io_r_sam_logon_ex("", &r_u, rdata, 0)) {
		DEBUG(0,("api_net_sam_logon_ex: Failed to marshall NET_R_SAM_LOGON_EX.\n"));
		return False;
	}

	return True;
}

/*******************************************************************
 array of \PIPE\NETLOGON operations
 ********************************************************************/
static struct api_struct api_net_cmds [] =
    {
      { "NET_REQCHAL"       , NET_REQCHAL       , api_net_req_chal       }, 
      { "NET_AUTH"          , NET_AUTH          , api_net_auth           }, 
      { "NET_AUTH2"         , NET_AUTH2         , api_net_auth_2         }, 
      { "NET_SRVPWSET"      , NET_SRVPWSET      , api_net_srv_pwset      }, 
      { "NET_SAMLOGON"      , NET_SAMLOGON      , api_net_sam_logon      }, 
      { "NET_SAMLOGOFF"     , NET_SAMLOGOFF     , api_net_sam_logoff     }, 
      { "NET_LOGON_CTRL2"   , NET_LOGON_CTRL2   , api_net_logon_ctrl2    }, 
      { "NET_TRUST_DOM_LIST", NET_TRUST_DOM_LIST, api_net_trust_dom_list },
      { "NET_LOGON_CTRL"    , NET_LOGON_CTRL    , api_net_logon_ctrl     },
      { "NET_SAMLOGON_EX"   , NET_SAMLOGON_EX   , api_net_sam_logon_ex   },
    };

void netlog_get_pipe_fns( struct api_struct **fns, int *n_fns )
{
	*fns = api_net_cmds;
	*n_fns = sizeof(api_net_cmds) / sizeof(struct api_struct);
}

NTSTATUS rpc_net_init(void)
{
  return rpc_pipe_register_commands(SMB_RPC_INTERFACE_VERSION, "NETLOGON", "lsass", api_net_cmds,
				    sizeof(api_net_cmds) / sizeof(struct api_struct));
}
