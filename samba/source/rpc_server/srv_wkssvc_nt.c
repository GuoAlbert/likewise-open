/* 
 *  Unix SMB/CIFS implementation.
 *  RPC Pipe client / server routines
 *
 *  Copyright (C) Andrew Tridgell		1992-1997,
 *  Copyright (C) Gerald (Jerry) Carter		2006.
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

/* This is the implementation of the wks interface. */

#include "includes.h"
#include "libnet/libnet.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_RPC_SRV

/*******************************************************************
 Fill in the values for the struct wkssvc_NetWkstaInfo100.
 ********************************************************************/

static void create_wks_info_100(struct wkssvc_NetWkstaInfo100 *info100)
{
	info100->platform_id	 = PLATFORM_ID_NT;	/* unknown */
	info100->version_major	 = lp_major_announce_version();
	info100->version_minor	 = lp_minor_announce_version();

	info100->server_name = talloc_asprintf_strupper_m(
		info100, "%s", global_myname());
	info100->domain_name = talloc_asprintf_strupper_m(
		info100, "%s", lp_workgroup());

	return;
}

/********************************************************************
 only supports info level 100 at the moment.
 ********************************************************************/

WERROR _wkssvc_NetWkstaGetInfo(pipes_struct *p, struct wkssvc_NetWkstaGetInfo *r)
{
	struct wkssvc_NetWkstaInfo100 *wks100 = NULL;
	
	/* We only support info level 100 currently */
	
	if ( r->in.level != 100 ) {
		return WERR_UNKNOWN_LEVEL;
	}

	if ( (wks100 = TALLOC_ZERO_P(p->mem_ctx, struct wkssvc_NetWkstaInfo100)) == NULL ) {
		return WERR_NOMEM;
	}

	create_wks_info_100( wks100 );
	
	r->out.info->info100 = wks100;

	return WERR_OK;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetWkstaSetInfo(pipes_struct *p, struct wkssvc_NetWkstaSetInfo *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetWkstaEnumUsers(pipes_struct *p, struct wkssvc_NetWkstaEnumUsers *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrWkstaUserGetInfo(pipes_struct *p, struct wkssvc_NetrWkstaUserGetInfo *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrWkstaUserSetInfo(pipes_struct *p, struct wkssvc_NetrWkstaUserSetInfo *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetWkstaTransportEnum(pipes_struct *p, struct wkssvc_NetWkstaTransportEnum *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrWkstaTransportAdd(pipes_struct *p, struct wkssvc_NetrWkstaTransportAdd *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrWkstaTransportDel(pipes_struct *p, struct wkssvc_NetrWkstaTransportDel *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrUseAdd(pipes_struct *p, struct wkssvc_NetrUseAdd *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrUseGetInfo(pipes_struct *p, struct wkssvc_NetrUseGetInfo *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrUseDel(pipes_struct *p, struct wkssvc_NetrUseDel *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrUseEnum(pipes_struct *p, struct wkssvc_NetrUseEnum *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrMessageBufferSend(pipes_struct *p, struct wkssvc_NetrMessageBufferSend *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrWorkstationStatisticsGet(pipes_struct *p, struct wkssvc_NetrWorkstationStatisticsGet *r) 
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrLogonDomainNameAdd(pipes_struct *p, struct wkssvc_NetrLogonDomainNameAdd *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrLogonDomainNameDel(pipes_struct *p, struct wkssvc_NetrLogonDomainNameDel *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrJoinDomain(pipes_struct *p, struct wkssvc_NetrJoinDomain *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrUnjoinDomain(pipes_struct *p, struct wkssvc_NetrUnjoinDomain *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrRenameMachineInDomain(pipes_struct *p, struct wkssvc_NetrRenameMachineInDomain *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrValidateName(pipes_struct *p, struct wkssvc_NetrValidateName *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrGetJoinInformation(pipes_struct *p, struct wkssvc_NetrGetJoinInformation *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrGetJoinableOus(pipes_struct *p, struct wkssvc_NetrGetJoinableOus *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrJoinDomain2(pipes_struct *p, struct wkssvc_NetrJoinDomain2 *r)
{
#if 0
	struct libnet_JoinCtx *j = NULL;
	char *cleartext_pwd = NULL;
	char *admin_domain = NULL;
	char *admin_account = NULL;
	WERROR werr;
	NTSTATUS status;
	struct nt_user_token *token = p->pipe_user.nt_user_token;
	struct DS_DOMAIN_CONTROLLER_INFO *info = NULL;

	if (!r->in.domain_name) {
		return WERR_INVALID_PARAM;
	}

	if (!user_has_privileges(token, &se_machine_account) &&
	    !nt_token_check_domain_rid(token, DOMAIN_GROUP_RID_ADMINS) &&
	    !nt_token_check_domain_rid(token, BUILTIN_ALIAS_RID_ADMINS)) {
		return WERR_ACCESS_DENIED;
	}

	werr = decode_wkssvc_join_password_buffer(p->mem_ctx,
						  r->in.encrypted_password,
						  &p->session_key,
						  &cleartext_pwd);
	if (!W_ERROR_IS_OK(werr)) {
		return werr;
	}

	split_domain_user(p->mem_ctx,
			  r->in.admin_account,
			  &admin_domain,
			  &admin_account);

	status = dsgetdcname(p->mem_ctx,
			     r->in.domain_name,
			     NULL,
			     NULL,
			     DS_DIRECTORY_SERVICE_REQUIRED |
			     DS_WRITABLE_REQUIRED |
			     DS_RETURN_DNS_NAME,
			     &info);
	if (!NT_STATUS_IS_OK(status)) {
		return ntstatus_to_werror(status);
	}

	werr = libnet_init_JoinCtx(p->mem_ctx, &j);
	if (!W_ERROR_IS_OK(werr)) {
		return werr;
	}

	j->in.dc_name		= info->domain_controller_name;
	j->in.domain_name	= r->in.domain_name;
	j->in.account_ou	= r->in.account_ou;
	j->in.join_flags	= r->in.join_flags;
	j->in.admin_account	= admin_account;
	j->in.admin_password	= cleartext_pwd;
	j->in.modify_config	= true;

	become_root();
	werr = libnet_Join(p->mem_ctx, j);
	unbecome_root();

	return werr;
#endif
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrUnjoinDomain2(pipes_struct *p, struct wkssvc_NetrUnjoinDomain2 *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrRenameMachineInDomain2(pipes_struct *p, struct wkssvc_NetrRenameMachineInDomain2 *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrValidateName2(pipes_struct *p, struct wkssvc_NetrValidateName2 *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrGetJoinableOus2(pipes_struct *p, struct wkssvc_NetrGetJoinableOus2 *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrAddAlternateComputerName(pipes_struct *p, struct wkssvc_NetrAddAlternateComputerName *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrRemoveAlternateComputerName(pipes_struct *p, struct wkssvc_NetrRemoveAlternateComputerName *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrSetPrimaryComputername(pipes_struct *p, struct wkssvc_NetrSetPrimaryComputername *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

/********************************************************************
 ********************************************************************/

WERROR _wkssvc_NetrEnumerateComputerNames(pipes_struct *p, struct wkssvc_NetrEnumerateComputerNames *r)
{
	/* FIXME: Add implementation code here */
	p->rng_fault_state = True;
	return WERR_NOT_SUPPORTED;
}

