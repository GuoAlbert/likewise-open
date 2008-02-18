/* 
 *  Unix SMB/CIFS implementation.
 *  RPC Pipe client / server routines
 *  Copyright (C) Andrew Tridgell              1992-1997,
 *  Copyright (C) Luke Kenneth Casson Leighton 1996-1997,
 *  Copyright (C) Paul Ashton                       1997,
 *  Copyright (C) Andrew Bartlett                   2002,
 *  Copyright (C) Jim McDonough <jmcd@us.ibm.com>   2002.
 *  Copyright (C) Gerald )Jerry) Carter             2005
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

#include "includes.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_RPC_PARSE

static bool lsa_io_trans_names(const char *desc, LSA_TRANS_NAME_ENUM *trn, prs_struct *ps, int depth);
static bool lsa_io_trans_names2(const char *desc, LSA_TRANS_NAME_ENUM2 *trn, prs_struct *ps, int depth);

/*******************************************************************
 Inits a LSA_TRANS_NAME structure.
********************************************************************/

void init_lsa_trans_name(LSA_TRANS_NAME *trn, UNISTR2 *uni_name,
			 uint16 sid_name_use, const char *name, uint32 idx)
{
	trn->sid_name_use = sid_name_use;
	init_unistr2(uni_name, name, UNI_FLAGS_NONE);
	init_uni_hdr(&trn->hdr_name, uni_name);
	trn->domain_idx = idx;
}

/*******************************************************************
 Reads or writes a LSA_TRANS_NAME structure.
********************************************************************/

static bool lsa_io_trans_name(const char *desc, LSA_TRANS_NAME *trn, prs_struct *ps, 
			      int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_trans_name");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!prs_uint16("sid_name_use", ps, depth, &trn->sid_name_use))
		return False;
	if(!prs_align(ps))
		return False;
	
	if(!smb_io_unihdr ("hdr_name", &trn->hdr_name, ps, depth))
		return False;
	if(!prs_uint32("domain_idx  ", ps, depth, &trn->domain_idx))
		return False;

	return True;
}

/*******************************************************************
 Inits a LSA_TRANS_NAME2 structure.
********************************************************************/

void init_lsa_trans_name2(LSA_TRANS_NAME2 *trn, UNISTR2 *uni_name,
			 uint16 sid_name_use, const char *name, uint32 idx)
{
	trn->sid_name_use = sid_name_use;
	init_unistr2(uni_name, name, UNI_FLAGS_NONE);
	init_uni_hdr(&trn->hdr_name, uni_name);
	trn->domain_idx = idx;
	trn->unknown = 0;
}

/*******************************************************************
 Reads or writes a LSA_TRANS_NAME2 structure.
********************************************************************/

static bool lsa_io_trans_name2(const char *desc, LSA_TRANS_NAME2 *trn, prs_struct *ps, 
			      int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_trans_name2");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!prs_uint16("sid_name_use", ps, depth, &trn->sid_name_use))
		return False;
	if(!prs_align(ps))
		return False;
	
	if(!smb_io_unihdr ("hdr_name", &trn->hdr_name, ps, depth))
		return False;
	if(!prs_uint32("domain_idx  ", ps, depth, &trn->domain_idx))
		return False;
	if(!prs_uint32("unknown  ", ps, depth, &trn->unknown))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes a DOM_R_REF structure.
********************************************************************/

static bool lsa_io_dom_r_ref(const char *desc, DOM_R_REF *dom, prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_dom_r_ref");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!prs_uint32("num_ref_doms_1", ps, depth, &dom->num_ref_doms_1)) /* num referenced domains? */
		return False;
	if(!prs_uint32("ptr_ref_dom   ", ps, depth, &dom->ptr_ref_dom)) /* undocumented buffer pointer. */
		return False;
	if(!prs_uint32("max_entries   ", ps, depth, &dom->max_entries)) /* 32 - max number of entries */
		return False;

	SMB_ASSERT_ARRAY(dom->hdr_ref_dom, dom->num_ref_doms_1);

	if (dom->ptr_ref_dom != 0) {

		if(!prs_uint32("num_ref_doms_2", ps, depth, &dom->num_ref_doms_2)) /* 4 - num referenced domains? */
			return False;

		SMB_ASSERT_ARRAY(dom->ref_dom, dom->num_ref_doms_2);

		for (i = 0; i < dom->num_ref_doms_1; i++) {
			fstring t;

			slprintf(t, sizeof(t) - 1, "dom_ref[%d] ", i);
			if(!smb_io_unihdr(t, &dom->hdr_ref_dom[i].hdr_dom_name, ps, depth))
				return False;

			slprintf(t, sizeof(t) - 1, "sid_ptr[%d] ", i);
			if(!prs_uint32(t, ps, depth, &dom->hdr_ref_dom[i].ptr_dom_sid))
				return False;
		}

		for (i = 0; i < dom->num_ref_doms_2; i++) {
			fstring t;

			if (dom->hdr_ref_dom[i].hdr_dom_name.buffer != 0) {
				slprintf(t, sizeof(t) - 1, "dom_ref[%d] ", i);
				if(!smb_io_unistr2(t, &dom->ref_dom[i].uni_dom_name, True, ps, depth)) /* domain name unicode string */
					return False;
				if(!prs_align(ps))
					return False;
			}

			if (dom->hdr_ref_dom[i].ptr_dom_sid != 0) {
				slprintf(t, sizeof(t) - 1, "sid_ptr[%d] ", i);
				if(!smb_io_dom_sid2(t, &dom->ref_dom[i].ref_dom, ps, depth)) /* referenced domain SIDs */
					return False;
			}
		}
	}

	return True;
}

/*******************************************************************
 Inits an LSA_SEC_QOS structure.
********************************************************************/

void init_lsa_sec_qos(LSA_SEC_QOS *qos, uint16 imp_lev, uint8 ctxt, uint8 eff)
{
	DEBUG(5, ("init_lsa_sec_qos\n"));

	qos->len = 0x0c; /* length of quality of service block, in bytes */
	qos->sec_imp_level = imp_lev;
	qos->sec_ctxt_mode = ctxt;
	qos->effective_only = eff;
}

/*******************************************************************
 Reads or writes an LSA_SEC_QOS structure.
********************************************************************/

static bool lsa_io_sec_qos(const char *desc,  LSA_SEC_QOS *qos, prs_struct *ps, 
			   int depth)
{
	uint32 start;

	prs_debug(ps, depth, desc, "lsa_io_obj_qos");
	depth++;

	if(!prs_align(ps))
		return False;
	
	start = prs_offset(ps);

	/* these pointers had _better_ be zero, because we don't know
	   what they point to!
	 */
	if(!prs_uint32("len           ", ps, depth, &qos->len)) /* 0x18 - length (in bytes) inc. the length field. */
		return False;
	if(!prs_uint16("sec_imp_level ", ps, depth, &qos->sec_imp_level ))
		return False;
	if(!prs_uint8 ("sec_ctxt_mode ", ps, depth, &qos->sec_ctxt_mode ))
		return False;
	if(!prs_uint8 ("effective_only", ps, depth, &qos->effective_only))
		return False;

	if (qos->len != prs_offset(ps) - start) {
		DEBUG(3,("lsa_io_sec_qos: length %x does not match size %x\n",
		         qos->len, prs_offset(ps) - start));
	}

	return True;
}

/*******************************************************************
 Inits an LSA_OBJ_ATTR structure.
********************************************************************/

static void init_lsa_obj_attr(LSA_OBJ_ATTR *attr, uint32 attributes, LSA_SEC_QOS *qos)
{
	DEBUG(5, ("init_lsa_obj_attr\n"));

	attr->len = 0x18; /* length of object attribute block, in bytes */
	attr->ptr_root_dir = 0;
	attr->ptr_obj_name = 0;
	attr->attributes = attributes;
	attr->ptr_sec_desc = 0;
	
	if (qos != NULL) {
		attr->ptr_sec_qos = 1;
		attr->sec_qos = qos;
	} else {
		attr->ptr_sec_qos = 0;
		attr->sec_qos = NULL;
	}
}

/*******************************************************************
 Reads or writes an LSA_OBJ_ATTR structure.
********************************************************************/

static bool lsa_io_obj_attr(const char *desc, LSA_OBJ_ATTR *attr, prs_struct *ps, 
			    int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_obj_attr");
	depth++;

	if(!prs_align(ps))
		return False;
	
	/* these pointers had _better_ be zero, because we don't know
	   what they point to!
	 */
	if(!prs_uint32("len         ", ps, depth, &attr->len)) /* 0x18 - length (in bytes) inc. the length field. */
		return False;
	if(!prs_uint32("ptr_root_dir", ps, depth, &attr->ptr_root_dir)) /* 0 - root directory (pointer) */
		return False;
	if(!prs_uint32("ptr_obj_name", ps, depth, &attr->ptr_obj_name)) /* 0 - object name (pointer) */
		return False;
	if(!prs_uint32("attributes  ", ps, depth, &attr->attributes)) /* 0 - attributes (undocumented) */
		return False;
	if(!prs_uint32("ptr_sec_desc", ps, depth, &attr->ptr_sec_desc)) /* 0 - security descriptior (pointer) */
		return False;
	if(!prs_uint32("ptr_sec_qos ", ps, depth, &attr->ptr_sec_qos )) /* security quality of service (pointer) */
		return False;

	if (attr->ptr_sec_qos != 0) {
		if (UNMARSHALLING(ps))
			if (!(attr->sec_qos = PRS_ALLOC_MEM(ps,LSA_SEC_QOS,1)))
				return False;

		if(!lsa_io_sec_qos("sec_qos", attr->sec_qos, ps, depth))
			return False;
	}

	return True;
}


/*******************************************************************
 Inits an LSA_Q_OPEN_POL structure.
********************************************************************/

void init_q_open_pol(LSA_Q_OPEN_POL *in, uint16 system_name,
		     uint32 attributes, uint32 desired_access,
		     LSA_SEC_QOS *qos)
{
	DEBUG(5, ("init_open_pol: attr:%d da:%d\n", attributes, 
		  desired_access));

	in->ptr = 1; /* undocumented pointer */

	in->des_access = desired_access;

	in->system_name = system_name;
	init_lsa_obj_attr(&in->attr, attributes, qos);
}

/*******************************************************************
 Reads or writes an LSA_Q_OPEN_POL structure.
********************************************************************/

bool lsa_io_q_open_pol(const char *desc, LSA_Q_OPEN_POL *in, prs_struct *ps, 
		       int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_open_pol");
	depth++;

	if(!prs_uint32("ptr       ", ps, depth, &in->ptr))
		return False;
	if(!prs_uint16("system_name", ps, depth, &in->system_name))
		return False;
	if(!prs_align( ps ))
		return False;

	if(!lsa_io_obj_attr("", &in->attr, ps, depth))
		return False;

	if(!prs_uint32("des_access", ps, depth, &in->des_access))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_OPEN_POL structure.
********************************************************************/

bool lsa_io_r_open_pol(const char *desc, LSA_R_OPEN_POL *out, prs_struct *ps, 
		       int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_open_pol");
	depth++;

	if(!smb_io_pol_hnd("", &out->pol, ps, depth))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Inits an LSA_Q_OPEN_POL2 structure.
********************************************************************/

void init_q_open_pol2(LSA_Q_OPEN_POL2 *in, const char *server_name,
			uint32 attributes, uint32 desired_access,
			LSA_SEC_QOS *qos)
{
	DEBUG(5, ("init_q_open_pol2: attr:%d da:%d\n", attributes, 
		  desired_access));

	in->ptr = 1; /* undocumented pointer */

	in->des_access = desired_access;

	init_unistr2(&in->uni_server_name, server_name, UNI_STR_TERMINATE);

	init_lsa_obj_attr(&in->attr, attributes, qos);
}

/*******************************************************************
 Reads or writes an LSA_Q_OPEN_POL2 structure.
********************************************************************/

bool lsa_io_q_open_pol2(const char *desc, LSA_Q_OPEN_POL2 *in, prs_struct *ps, 
			int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_open_pol2");
	depth++;

	if(!prs_uint32("ptr       ", ps, depth, &in->ptr))
		return False;

	if(!smb_io_unistr2 ("", &in->uni_server_name, in->ptr, ps, depth))
		return False;
	if(!lsa_io_obj_attr("", &in->attr, ps, depth))
		return False;

	if(!prs_uint32("des_access", ps, depth, &in->des_access))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_OPEN_POL2 structure.
********************************************************************/

bool lsa_io_r_open_pol2(const char *desc, LSA_R_OPEN_POL2 *out, prs_struct *ps, 
			int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_open_pol2");
	depth++;

	if(!smb_io_pol_hnd("", &out->pol, ps, depth))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
makes an LSA_Q_QUERY_SEC_OBJ structure.
********************************************************************/

void init_q_query_sec_obj(LSA_Q_QUERY_SEC_OBJ *in, const POLICY_HND *hnd, 
			  uint32 sec_info)
{
	DEBUG(5, ("init_q_query_sec_obj\n"));

	in->pol = *hnd;
	in->sec_info = sec_info;

	return;
}

/*******************************************************************
 Reads or writes an LSA_Q_QUERY_SEC_OBJ structure.
********************************************************************/

bool lsa_io_q_query_sec_obj(const char *desc, LSA_Q_QUERY_SEC_OBJ *in, 
			    prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_query_sec_obj");
	depth++;

	if (!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if (!prs_uint32("sec_info", ps, depth, &in->sec_info))
		return False;

	return True;
} 

/*******************************************************************
 Reads or writes a LSA_R_QUERY_SEC_OBJ structure.
********************************************************************/

bool lsa_io_r_query_sec_obj(const char *desc, LSA_R_QUERY_SEC_OBJ *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_query_sec_obj");
	depth++;

	if (!prs_align(ps))
		return False;

	if (!prs_uint32("ptr", ps, depth, &out->ptr))
		return False;

	if (out->ptr != 0) {
		if (!sec_io_desc_buf("sec", &out->buf, ps, depth))
			return False;
	}

	if (!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Inits an LSA_Q_QUERY_INFO structure.
********************************************************************/

void init_q_query(LSA_Q_QUERY_INFO *in, POLICY_HND *hnd, uint16 info_class)
{
	DEBUG(5, ("init_q_query\n"));

	memcpy(&in->pol, hnd, sizeof(in->pol));

	in->info_class = info_class;
}

/*******************************************************************
 Reads or writes an LSA_Q_QUERY_INFO structure.
********************************************************************/

bool lsa_io_q_query(const char *desc, LSA_Q_QUERY_INFO *in, prs_struct *ps, 
		    int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_query");
	depth++;

	if(!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if(!prs_uint16("info_class", ps, depth, &in->info_class))
		return False;

	return True;
}

/*******************************************************************
makes an LSA_Q_ENUM_TRUST_DOM structure.
********************************************************************/
bool init_q_enum_trust_dom(LSA_Q_ENUM_TRUST_DOM * q_e, POLICY_HND *pol,
			   uint32 enum_context, uint32 preferred_len)
{
	DEBUG(5, ("init_q_enum_trust_dom\n"));

	q_e->pol = *pol;
	q_e->enum_context = enum_context;
	q_e->preferred_len = preferred_len;

	return True;
}

/*******************************************************************
 Reads or writes an LSA_Q_ENUM_TRUST_DOM structure.
********************************************************************/

bool lsa_io_q_enum_trust_dom(const char *desc, LSA_Q_ENUM_TRUST_DOM *q_e, 
			     prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_enum_trust_dom");
	depth++;

	if(!smb_io_pol_hnd("", &q_e->pol, ps, depth))
		return False;

	if(!prs_uint32("enum_context ", ps, depth, &q_e->enum_context))
		return False;
	if(!prs_uint32("preferred_len", ps, depth, &q_e->preferred_len))
		return False;

	return True;
}

/*******************************************************************
 Inits an LSA_R_ENUM_TRUST_DOM structure.
********************************************************************/

void init_r_enum_trust_dom(TALLOC_CTX *ctx, LSA_R_ENUM_TRUST_DOM *out,
			   uint32 enum_context, uint32 num_domains,
			   struct trustdom_info **td)
{
	unsigned int i;

        DEBUG(5, ("init_r_enum_trust_dom\n"));
	
        out->enum_context  = enum_context;
	out->count         = num_domains;
			
	if ( num_domains != 0 ) {
	
		/* allocate container memory */
		
		out->domlist = TALLOC_P( ctx, DOMAIN_LIST );

		if ( !out->domlist ) {
			out->status = NT_STATUS_NO_MEMORY;
			return;
		}

		if (out->count) {
			out->domlist->domains = TALLOC_ARRAY( ctx, DOMAIN_INFO,
						      out->count );
			if ( !out->domlist->domains ) {
				out->status = NT_STATUS_NO_MEMORY;
				return;
			}
		} else {		
			out->domlist->domains = NULL;
		}
	
		out->domlist->count = out->count;
		
		/* initialize the list of domains and their sid */
		
		for (i = 0; i < num_domains; i++) {	
			smb_ucs2_t *name;
			if ( !(out->domlist->domains[i].sid =
			       TALLOC_P(ctx, DOM_SID2)) ) {
				out->status = NT_STATUS_NO_MEMORY;
				return;
			}
				
			init_dom_sid2(out->domlist->domains[i].sid,
				      &(td[i])->sid);
			if (push_ucs2_talloc(ctx, &name, (td[i])->name) == (size_t)-1){
				out->status = NT_STATUS_NO_MEMORY;
				return;
			}
			init_unistr4_w(ctx, &out->domlist->domains[i].name,
				       name);
		}
	}

}

/*******************************************************************
********************************************************************/

bool lsa_io_domain_list( const char *desc, prs_struct *ps, int depth, DOMAIN_LIST *domlist )
{
	int i;
	
	prs_debug(ps, depth, desc, "lsa_io_domain_list");
	depth++;

	if(!prs_uint32("count", ps, depth, &domlist->count))
		return False;

	if ( domlist->count == 0 )
		return True;
		
	if ( UNMARSHALLING(ps) ) {
		if ( !(domlist->domains = PRS_ALLOC_MEM( ps, DOMAIN_INFO, domlist->count )) )
			return False;
	}
	
	/* headers */
	
	for ( i=0; i<domlist->count; i++ ) {
		if ( !prs_unistr4_hdr("name_header", ps, depth, &domlist->domains[i].name) )
			return False;
		if ( !smb_io_dom_sid2_p("sid_header", ps, depth, &domlist->domains[i].sid) )
			return False;
	}

	/* data */
	
	for ( i=0; i<domlist->count; i++ ) {
		if ( !prs_unistr4_str("name", ps, depth, &domlist->domains[i].name) )
			return False;
		if( !smb_io_dom_sid2("sid", domlist->domains[i].sid, ps, depth) )
			return False;
	}
	
	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_ENUM_TRUST_DOM structure.
********************************************************************/

bool lsa_io_r_enum_trust_dom(const char *desc, LSA_R_ENUM_TRUST_DOM *out, 
			     prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_enum_trust_dom");
	depth++;

	if(!prs_uint32("enum_context", ps, depth, &out->enum_context))
		return False;

	if(!prs_uint32("count", ps, depth, &out->count))
		return False;

	if ( !prs_pointer("trusted_domains", ps, depth, (void*)&out->domlist, sizeof(DOMAIN_LIST), (PRS_POINTER_CAST)lsa_io_domain_list))
		return False;
		
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/

static bool lsa_io_dom_query_1(const char *desc, DOM_QUERY_1 *d_q, prs_struct *ps, int depth)
{
	if (d_q == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_dom_query_1");
	depth++;

	if (!prs_align(ps))
		return False;

	if (!prs_uint32("percent_full", ps, depth, &d_q->percent_full))
		return False;
	if (!prs_uint32("log_size", ps, depth, &d_q->log_size))
		return False;
	if (!smb_io_nttime("retention_time", ps, depth, &d_q->retention_time))
		return False;
	if (!prs_uint8("shutdown_in_progress", ps, depth, &d_q->shutdown_in_progress))
		return False;
	if (!smb_io_nttime("time_to_shutdown", ps, depth, &d_q->time_to_shutdown))
		return False;
	if (!prs_uint32("next_audit_record", ps, depth, &d_q->next_audit_record))
		return False;
	if (!prs_uint32("unknown", ps, depth, &d_q->unknown))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/

static bool lsa_io_dom_query_2(const char *desc, DOM_QUERY_2 *d_q, prs_struct *ps, int depth)
{
	if (d_q == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_dom_query_2");
	depth++;

	if (!prs_align(ps))
		return False;

	if (!prs_uint32("auditing_enabled", ps, depth, &d_q->auditing_enabled))
		return False;
	if (!prs_uint32("ptr   ", ps, depth, &d_q->ptr))
		return False;
	if (!prs_uint32("count1", ps, depth, &d_q->count1))
		return False;

	if (d_q->ptr) {

		if (!prs_uint32("count2", ps, depth, &d_q->count2))
			return False;

		if (d_q->count1 != d_q->count2)
			return False;

		if (UNMARSHALLING(ps)) {
			if (d_q->count2) {
				d_q->auditsettings = TALLOC_ZERO_ARRAY(ps->mem_ctx, uint32, d_q->count2);
				if (!d_q->auditsettings) {
					return False;
				}
			} else {
				d_q->auditsettings = NULL;
			}
		}

		if (!prs_uint32s(False, "auditsettings", ps, depth, d_q->auditsettings, d_q->count2))
			return False;
	}

	return True;
}

/*******************************************************************
reads or writes a dom query structure.
********************************************************************/

static bool lsa_io_dom_query_3(const char *desc, DOM_QUERY_3 *d_q, prs_struct *ps, int depth)
{
	if (d_q == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_dom_query_3");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint16("uni_dom_max_len", ps, depth, &d_q->uni_dom_max_len)) /* domain name string length * 2 */
		return False;
	if(!prs_uint16("uni_dom_str_len", ps, depth, &d_q->uni_dom_str_len)) /* domain name string length * 2 */
		return False;

	if(!prs_uint32("buffer_dom_name", ps, depth, &d_q->buffer_dom_name)) /* undocumented domain name string buffer pointer */
		return False;
	if(!prs_uint32("buffer_dom_sid ", ps, depth, &d_q->buffer_dom_sid)) /* undocumented domain SID string buffer pointer */
		return False;

	if(!smb_io_unistr2("unistr2", &d_q->uni_domain_name, d_q->buffer_dom_name, ps, depth)) /* domain name (unicode string) */
		return False;

	if(!prs_align(ps))
		return False;

	if (d_q->buffer_dom_sid != 0) {
		if(!smb_io_dom_sid2("", &d_q->dom_sid, ps, depth)) /* domain SID */
			return False;
	} else {
		memset((char *)&d_q->dom_sid, '\0', sizeof(d_q->dom_sid));
	}

	return True;
}

/*******************************************************************
 Reads or writes a dom query structure.
********************************************************************/

static bool lsa_io_dom_query_5(const char *desc, DOM_QUERY_5 *d_q, prs_struct *ps, int depth)
{
	return lsa_io_dom_query_3("", d_q, ps, depth);
}

/*******************************************************************
 Reads or writes a dom query structure.
********************************************************************/

static bool lsa_io_dom_query_6(const char *desc, DOM_QUERY_6 *d_q, prs_struct *ps, int depth)
{
	if (d_q == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_dom_query_6");
	depth++;

	if (!prs_uint16("server_role", ps, depth, &d_q->server_role))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes a dom query structure.
********************************************************************/

static bool lsa_io_dom_query_10(const char *desc, DOM_QUERY_10 *d_q, prs_struct *ps, int depth)
{
	if (d_q == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_dom_query_10");
	depth++;

	if (!prs_uint8("shutdown_on_full", ps, depth, &d_q->shutdown_on_full))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes a dom query structure.
********************************************************************/

static bool lsa_io_dom_query_11(const char *desc, DOM_QUERY_11 *d_q, prs_struct *ps, int depth)
{
	if (d_q == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_dom_query_11");
	depth++;

	if (!prs_uint16("unknown", ps, depth, &d_q->unknown))
		return False;
	if (!prs_uint8("shutdown_on_full", ps, depth, &d_q->shutdown_on_full))
		return False;
	if (!prs_uint8("log_is_full", ps, depth, &d_q->log_is_full))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LSA_DNS_DOM_INFO structure.
********************************************************************/

bool lsa_io_dom_query_12(const char *desc, DOM_QUERY_12 *info, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_dom_query_12");
	depth++;

	if(!prs_align(ps))
		return False;
	if(!smb_io_unihdr("nb_name", &info->hdr_nb_dom_name, ps, depth))
		return False;
	if(!smb_io_unihdr("dns_name", &info->hdr_dns_dom_name, ps, depth))
		return False;
	if(!smb_io_unihdr("forest", &info->hdr_forest_name, ps, depth))
		return False;

	if(!prs_align(ps))
		return False;
	if ( !smb_io_uuid("dom_guid", &info->dom_guid, ps, depth) )
		return False;

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("dom_sid", ps, depth, &info->ptr_dom_sid))
		return False;

	if(!smb_io_unistr2("nb_name", &info->uni_nb_dom_name,
			   info->hdr_nb_dom_name.buffer, ps, depth))
		return False;
	if(!smb_io_unistr2("dns_name", &info->uni_dns_dom_name, 
			   info->hdr_dns_dom_name.buffer, ps, depth))
		return False;
	if(!smb_io_unistr2("forest", &info->uni_forest_name, 
			   info->hdr_forest_name.buffer, ps, depth))
		return False;

	if(!smb_io_dom_sid2("dom_sid", &info->dom_sid, ps, depth))
		return False;

	return True;
	
}


/*******************************************************************
reads or writes a structure.
********************************************************************/

static bool lsa_io_query_info_ctr2(const char *desc, prs_struct *ps, int depth, LSA_INFO_CTR2 *ctr)
{
	prs_debug(ps, depth, desc, "lsa_io_query_info_ctr2");
	depth++;

	if(!prs_uint16("info_class", ps, depth, &ctr->info_class))
		return False;

	switch (ctr->info_class) {
	case 1:
		if(!lsa_io_dom_query_1("", &ctr->info.id1, ps, depth))
			return False;
		break;
	case 2:
		if(!lsa_io_dom_query_2("", &ctr->info.id2, ps, depth))
			return False;
		break;
	case 3:
		if(!lsa_io_dom_query_3("", &ctr->info.id3, ps, depth))
			return False;
		break;
	case 5:
		if(!lsa_io_dom_query_5("", &ctr->info.id5, ps, depth))
			return False;
		break;
	case 6:
		if(!lsa_io_dom_query_6("", &ctr->info.id6, ps, depth))
			return False;
		break;
	case 10:
		if(!lsa_io_dom_query_10("", &ctr->info.id10, ps, depth))
			return False;
		break;
	case 11:
		if(!lsa_io_dom_query_11("", &ctr->info.id11, ps, depth))
			return False;
		break;
	case 12:
		if(!lsa_io_dom_query_12("", &ctr->info.id12, ps, depth))
			return False;
		break;
	default:
		DEBUG(0,("invalid info_class: %d\n", ctr->info_class));
		return False;
		break;
	}

	return True;
}


/*******************************************************************
reads or writes a structure.
********************************************************************/

static bool lsa_io_query_info_ctr(const char *desc, prs_struct *ps, int depth, LSA_INFO_CTR *ctr)
{
	prs_debug(ps, depth, desc, "lsa_io_query_info_ctr");
	depth++;

	if(!prs_uint16("info_class", ps, depth, &ctr->info_class))
		return False;

	if(!prs_align(ps))
		return False;

	switch (ctr->info_class) {
	case 1:
		if(!lsa_io_dom_query_1("", &ctr->info.id1, ps, depth))
			return False;
		break;
	case 2:
		if(!lsa_io_dom_query_2("", &ctr->info.id2, ps, depth))
			return False;
		break;
	case 3:
		if(!lsa_io_dom_query_3("", &ctr->info.id3, ps, depth))
			return False;
		break;
	case 5:
		if(!lsa_io_dom_query_5("", &ctr->info.id5, ps, depth))
			return False;
		break;
	case 6:
		if(!lsa_io_dom_query_6("", &ctr->info.id6, ps, depth))
			return False;
		break;
	case 10:
		if(!lsa_io_dom_query_10("", &ctr->info.id10, ps, depth))
			return False;
		break;
	case 11:
		if(!lsa_io_dom_query_11("", &ctr->info.id11, ps, depth))
			return False;
		break;
	default:
		DEBUG(0,("invalid info_class: %d\n", ctr->info_class));
		return False;
		break;
	}

	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_QUERY_INFO structure.
********************************************************************/

bool lsa_io_r_query(const char *desc, LSA_R_QUERY_INFO *out, prs_struct *ps, int depth)
{

	prs_debug(ps, depth, desc, "lsa_io_r_query");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("dom_ptr", ps, depth, &out->dom_ptr))
		return False;

	if (out->dom_ptr) {

		if(!lsa_io_query_info_ctr("", ps, depth, &out->ctr))
			return False;
	}

	if(!prs_align(ps))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Inits a LSA_SID_ENUM structure.
********************************************************************/

static void init_lsa_sid_enum(TALLOC_CTX *mem_ctx, LSA_SID_ENUM *sen, 
		       int num_entries, const DOM_SID *sids)
{
	int i;

	DEBUG(5, ("init_lsa_sid_enum\n"));

	sen->num_entries  = num_entries;
	sen->ptr_sid_enum = (num_entries != 0);
	sen->num_entries2 = num_entries;

	/* Allocate memory for sids and sid pointers */

	if (num_entries) {
		if ((sen->ptr_sid = TALLOC_ZERO_ARRAY(mem_ctx, uint32, num_entries )) == NULL) {
			DEBUG(3, ("init_lsa_sid_enum(): out of memory for ptr_sid\n"));
			return;
		}

		if ((sen->sid = TALLOC_ZERO_ARRAY(mem_ctx, DOM_SID2, num_entries)) == NULL) {
			DEBUG(3, ("init_lsa_sid_enum(): out of memory for sids\n"));
			return;
		}
	}

	/* Copy across SIDs and SID pointers */

	for (i = 0; i < num_entries; i++) {
		sen->ptr_sid[i] = 1;
		init_dom_sid2(&sen->sid[i], &sids[i]);
	}
}

/*******************************************************************
 Reads or writes a LSA_SID_ENUM structure.
********************************************************************/

static bool lsa_io_sid_enum(const char *desc, LSA_SID_ENUM *sen, prs_struct *ps, 
			    int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_sid_enum");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!prs_uint32("num_entries ", ps, depth, &sen->num_entries))
		return False;
	if(!prs_uint32("ptr_sid_enum", ps, depth, &sen->ptr_sid_enum))
		return False;

	/*
	   if the ptr is NULL, leave here. checked from a real w2k trace.
	   JFM, 11/23/2001
	 */
	
	if (sen->ptr_sid_enum==0)
		return True;

	if(!prs_uint32("num_entries2", ps, depth, &sen->num_entries2))
		return False;

	/* Mallocate memory if we're unpacking from the wire */

	if (UNMARSHALLING(ps) && sen->num_entries) {
		if ((sen->ptr_sid = PRS_ALLOC_MEM( ps, uint32, sen->num_entries)) == NULL) {
			DEBUG(3, ("init_lsa_sid_enum(): out of memory for "
				  "ptr_sid\n"));
			return False;
		}

		if ((sen->sid = PRS_ALLOC_MEM( ps, DOM_SID2, sen->num_entries)) == NULL) {
			DEBUG(3, ("init_lsa_sid_enum(): out of memory for "
				  "sids\n"));
			return False;
		}
	}

	for (i = 0; i < sen->num_entries; i++) {	
		fstring temp;

		slprintf(temp, sizeof(temp) - 1, "ptr_sid[%d]", i);
		if(!prs_uint32(temp, ps, depth, &sen->ptr_sid[i])) {
			return False;
		}
	}

	for (i = 0; i < sen->num_entries; i++) {
		fstring temp;

		slprintf(temp, sizeof(temp) - 1, "sid[%d]", i);
		if(!smb_io_dom_sid2(temp, &sen->sid[i], ps, depth)) {
			return False;
		}
	}

	return True;
}

/*******************************************************************
 Inits an LSA_R_ENUM_TRUST_DOM structure.
********************************************************************/

void init_q_lookup_sids(TALLOC_CTX *mem_ctx, LSA_Q_LOOKUP_SIDS *q_l, 
			POLICY_HND *hnd, int num_sids, const DOM_SID *sids,
			uint16 level)
{
	DEBUG(5, ("init_q_lookup_sids\n"));

	ZERO_STRUCTP(q_l);

	memcpy(&q_l->pol, hnd, sizeof(q_l->pol));
	init_lsa_sid_enum(mem_ctx, &q_l->sids, num_sids, sids);
	
	q_l->level = level;
}

/*******************************************************************
 Reads or writes a LSA_Q_LOOKUP_SIDS structure.
********************************************************************/

bool lsa_io_q_lookup_sids(const char *desc, LSA_Q_LOOKUP_SIDS *q_s, prs_struct *ps,
			  int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_lookup_sids");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!smb_io_pol_hnd("pol_hnd", &q_s->pol, ps, depth)) /* policy handle */
		return False;
	if(!lsa_io_sid_enum("sids   ", &q_s->sids, ps, depth)) /* sids to be looked up */
		return False;
	if(!lsa_io_trans_names("names  ", &q_s->names, ps, depth)) /* translated names */
		return False;

	if(!prs_uint16("level", ps, depth, &q_s->level)) /* lookup level */
		return False;
	if(!prs_align(ps))
		return False;

	if(!prs_uint32("mapped_count", ps, depth, &q_s->mapped_count))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes a LSA_Q_LOOKUP_SIDS2 structure.
********************************************************************/

bool lsa_io_q_lookup_sids2(const char *desc, LSA_Q_LOOKUP_SIDS2 *q_s, prs_struct *ps,
			  int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_lookup_sids2");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!smb_io_pol_hnd("pol_hnd", &q_s->pol, ps, depth)) /* policy handle */
		return False;
	if(!lsa_io_sid_enum("sids   ", &q_s->sids, ps, depth)) /* sids to be looked up */
		return False;
	if(!lsa_io_trans_names2("names  ", &q_s->names, ps, depth)) /* translated names */
		return False;

	if(!prs_uint16("level", ps, depth, &q_s->level)) /* lookup level */
		return False;
	if(!prs_align(ps))
		return False;

	if(!prs_uint32("mapped_count", ps, depth, &q_s->mapped_count))
		return False;
	if(!prs_uint32("unknown1", ps, depth, &q_s->unknown1))
		return False;
	if(!prs_uint32("unknown2", ps, depth, &q_s->unknown2))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes a LSA_Q_LOOKUP_SIDS3 structure.
********************************************************************/

bool lsa_io_q_lookup_sids3(const char *desc, LSA_Q_LOOKUP_SIDS3 *q_s, prs_struct *ps,
			  int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_lookup_sids3");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!lsa_io_sid_enum("sids   ", &q_s->sids, ps, depth)) /* sids to be looked up */
		return False;
	if(!lsa_io_trans_names2("names  ", &q_s->names, ps, depth)) /* translated names */
		return False;

	if(!prs_uint16("level", ps, depth, &q_s->level)) /* lookup level */
		return False;
	if(!prs_align(ps))
		return False;

	if(!prs_uint32("mapped_count", ps, depth, &q_s->mapped_count))
		return False;
	if(!prs_uint32("unknown1", ps, depth, &q_s->unknown1))
		return False;
	if(!prs_uint32("unknown2", ps, depth, &q_s->unknown2))
		return False;

	return True;
}


/*******************************************************************
 Reads or writes a structure.
********************************************************************/

static bool lsa_io_trans_names(const char *desc, LSA_TRANS_NAME_ENUM *trn,
                prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_trans_names");
	depth++;

	if(!prs_align(ps))
		return False;
   
	if(!prs_uint32("num_entries    ", ps, depth, &trn->num_entries))
		return False;
	if(!prs_uint32("ptr_trans_names", ps, depth, &trn->ptr_trans_names))
		return False;

	if (trn->ptr_trans_names != 0) {
		if(!prs_uint32("num_entries2   ", ps, depth, 
			       &trn->num_entries2))
			return False;

		if (trn->num_entries2 != trn->num_entries) {
			/* RPC fault */
			return False;
		}

		if (UNMARSHALLING(ps) && trn->num_entries2) {
			if ((trn->name = PRS_ALLOC_MEM(ps, LSA_TRANS_NAME, trn->num_entries2)) == NULL) {
				return False;
			}

			if ((trn->uni_name = PRS_ALLOC_MEM(ps, UNISTR2, trn->num_entries2)) == NULL) {
				return False;
			}
		}

		for (i = 0; i < trn->num_entries2; i++) {
			fstring t;
			slprintf(t, sizeof(t) - 1, "name[%d] ", i);

			if(!lsa_io_trans_name(t, &trn->name[i], ps, depth)) /* translated name */
				return False;
		}

		for (i = 0; i < trn->num_entries2; i++) {
			fstring t;
			slprintf(t, sizeof(t) - 1, "name[%d] ", i);

			if(!smb_io_unistr2(t, &trn->uni_name[i], trn->name[i].hdr_name.buffer, ps, depth))
				return False;
			if(!prs_align(ps))
				return False;
		}
	}

	return True;
}

/*******************************************************************
 Reads or writes a structure.
********************************************************************/

static bool lsa_io_trans_names2(const char *desc, LSA_TRANS_NAME_ENUM2 *trn,
                prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_trans_names2");
	depth++;

	if(!prs_align(ps))
		return False;
   
	if(!prs_uint32("num_entries    ", ps, depth, &trn->num_entries))
		return False;
	if(!prs_uint32("ptr_trans_names", ps, depth, &trn->ptr_trans_names))
		return False;

	if (trn->ptr_trans_names != 0) {
		if(!prs_uint32("num_entries2   ", ps, depth, 
			       &trn->num_entries2))
			return False;

		if (trn->num_entries2 != trn->num_entries) {
			/* RPC fault */
			return False;
		}

		if (UNMARSHALLING(ps) && trn->num_entries2) {
			if ((trn->name = PRS_ALLOC_MEM(ps, LSA_TRANS_NAME2, trn->num_entries2)) == NULL) {
				return False;
			}

			if ((trn->uni_name = PRS_ALLOC_MEM(ps, UNISTR2, trn->num_entries2)) == NULL) {
				return False;
			}
		}

		for (i = 0; i < trn->num_entries2; i++) {
			fstring t;
			slprintf(t, sizeof(t) - 1, "name[%d] ", i);

			if(!lsa_io_trans_name2(t, &trn->name[i], ps, depth)) /* translated name */
				return False;
		}

		for (i = 0; i < trn->num_entries2; i++) {
			fstring t;
			slprintf(t, sizeof(t) - 1, "name[%d] ", i);

			if(!smb_io_unistr2(t, &trn->uni_name[i], trn->name[i].hdr_name.buffer, ps, depth))
				return False;
			if(!prs_align(ps))
				return False;
		}
	}

	return True;
}


/*******************************************************************
 Reads or writes a structure.
********************************************************************/

bool lsa_io_r_lookup_sids(const char *desc, LSA_R_LOOKUP_SIDS *r_s, 
			  prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_lookup_sids");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!prs_uint32("ptr_dom_ref", ps, depth, &r_s->ptr_dom_ref))
		return False;

	if (r_s->ptr_dom_ref != 0)
		if(!lsa_io_dom_r_ref ("dom_ref", r_s->dom_ref, ps, depth)) /* domain reference info */
			return False;

	if(!lsa_io_trans_names("names  ", &r_s->names, ps, depth)) /* translated names */
		return False;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("mapped_count", ps, depth, &r_s->mapped_count))
		return False;

	if(!prs_ntstatus("status      ", ps, depth, &r_s->status))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes a structure.
********************************************************************/

bool lsa_io_r_lookup_sids2(const char *desc, LSA_R_LOOKUP_SIDS2 *r_s, 
			  prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_lookup_sids2");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!prs_uint32("ptr_dom_ref", ps, depth, &r_s->ptr_dom_ref))
		return False;

	if (r_s->ptr_dom_ref != 0)
		if(!lsa_io_dom_r_ref ("dom_ref", r_s->dom_ref, ps, depth)) /* domain reference info */
			return False;

	if(!lsa_io_trans_names2("names  ", &r_s->names, ps, depth)) /* translated names */
		return False;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("mapped_count", ps, depth, &r_s->mapped_count))
		return False;

	if(!prs_ntstatus("status      ", ps, depth, &r_s->status))
		return False;

	return True;
}


/*******************************************************************
 Reads or writes a structure.
********************************************************************/

bool lsa_io_r_lookup_sids3(const char *desc, LSA_R_LOOKUP_SIDS3 *r_s, 
			  prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_lookup_sids3");
	depth++;

	if(!prs_align(ps))
		return False;
	
	if(!prs_uint32("ptr_dom_ref", ps, depth, &r_s->ptr_dom_ref))
		return False;

	if (r_s->ptr_dom_ref != 0)
		if(!lsa_io_dom_r_ref ("dom_ref", r_s->dom_ref, ps, depth)) /* domain reference info */
			return False;

	if(!lsa_io_trans_names2("names  ", &r_s->names, ps, depth)) /* translated names */
		return False;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("mapped_count", ps, depth, &r_s->mapped_count))
		return False;

	if(!prs_ntstatus("status      ", ps, depth, &r_s->status))
		return False;

	return True;
}

/*******************************************************************
makes a structure.
********************************************************************/

void init_q_lookup_names(TALLOC_CTX *mem_ctx, LSA_Q_LOOKUP_NAMES *q_l, 
			 POLICY_HND *hnd, int num_names, const char **names, 
			 int level)
{
	unsigned int i;

	DEBUG(5, ("init_q_lookup_names\n"));

	ZERO_STRUCTP(q_l);

	q_l->pol = *hnd;
	q_l->num_entries = num_names;
	q_l->num_entries2 = num_names;
	q_l->lookup_level = level;

	if (num_names) {
		if ((q_l->uni_name = TALLOC_ZERO_ARRAY(mem_ctx, UNISTR2, num_names)) == NULL) {
			DEBUG(3, ("init_q_lookup_names(): out of memory\n"));
			return;
		}

		if ((q_l->hdr_name = TALLOC_ZERO_ARRAY(mem_ctx, UNIHDR, num_names)) == NULL) {
			DEBUG(3, ("init_q_lookup_names(): out of memory\n"));
			return;
		}
	} else {
		q_l->uni_name = NULL;
		q_l->hdr_name = NULL;
	}

	for (i = 0; i < num_names; i++) {
		init_unistr2(&q_l->uni_name[i], names[i], UNI_FLAGS_NONE);
		init_uni_hdr(&q_l->hdr_name[i], &q_l->uni_name[i]);
	}
}

/*******************************************************************
reads or writes a structure.
********************************************************************/

bool lsa_io_q_lookup_names(const char *desc, LSA_Q_LOOKUP_NAMES *q_r, 
			   prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_q_lookup_names");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!smb_io_pol_hnd("", &q_r->pol, ps, depth)) /* policy handle */
		return False;

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("num_entries    ", ps, depth, &q_r->num_entries))
		return False;
	if(!prs_uint32("num_entries2   ", ps, depth, &q_r->num_entries2))
		return False;

	if (UNMARSHALLING(ps)) {
		if (q_r->num_entries) {
			if ((q_r->hdr_name = PRS_ALLOC_MEM(ps, UNIHDR, q_r->num_entries)) == NULL)
				return False;
			if ((q_r->uni_name = PRS_ALLOC_MEM(ps, UNISTR2, q_r->num_entries)) == NULL)
				return False;
		}
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unihdr("hdr_name", &q_r->hdr_name[i], ps, depth)) /* pointer names */
			return False;
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unistr2("dom_name", &q_r->uni_name[i], q_r->hdr_name[i].buffer, ps, depth)) /* names to be looked up */
			return False;
	}

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("num_trans_entries ", ps, depth, &q_r->num_trans_entries))
		return False;
	if(!prs_uint32("ptr_trans_sids ", ps, depth, &q_r->ptr_trans_sids))
		return False;
	if(!prs_uint16("lookup_level   ", ps, depth, &q_r->lookup_level))
		return False;
	if(!prs_align(ps))
		return False;
	if(!prs_uint32("mapped_count   ", ps, depth, &q_r->mapped_count))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/

bool lsa_io_r_lookup_names(const char *desc, LSA_R_LOOKUP_NAMES *out, prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_r_lookup_names");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("ptr_dom_ref", ps, depth, &out->ptr_dom_ref))
		return False;

	if (out->ptr_dom_ref != 0)
		if(!lsa_io_dom_r_ref("", out->dom_ref, ps, depth))
			return False;

	if(!prs_uint32("num_entries", ps, depth, &out->num_entries))
		return False;
	if(!prs_uint32("ptr_entries", ps, depth, &out->ptr_entries))
		return False;

	if (out->ptr_entries != 0) {
		if(!prs_uint32("num_entries2", ps, depth, &out->num_entries2))
			return False;

		if (out->num_entries2 != out->num_entries) {
			/* RPC fault */
			return False;
		}

		if (UNMARSHALLING(ps) && out->num_entries2) {
			if ((out->dom_rid = PRS_ALLOC_MEM(ps, DOM_RID, out->num_entries2))
			    == NULL) {
				DEBUG(3, ("lsa_io_r_lookup_names(): out of memory\n"));
				return False;
			}
		}

		for (i = 0; i < out->num_entries2; i++)
			if(!smb_io_dom_rid("", &out->dom_rid[i], ps, depth)) /* domain RIDs being looked up */
				return False;
	}

	if(!prs_uint32("mapped_count", ps, depth, &out->mapped_count))
		return False;

	if(!prs_ntstatus("status      ", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/

bool lsa_io_q_lookup_names2(const char *desc, LSA_Q_LOOKUP_NAMES2 *q_r, 
			   prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_q_lookup_names2");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!smb_io_pol_hnd("", &q_r->pol, ps, depth)) /* policy handle */
		return False;

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("num_entries    ", ps, depth, &q_r->num_entries))
		return False;
	if(!prs_uint32("num_entries2   ", ps, depth, &q_r->num_entries2))
		return False;

	if (UNMARSHALLING(ps)) {
		if (q_r->num_entries) {
			if ((q_r->hdr_name = PRS_ALLOC_MEM(ps, UNIHDR, q_r->num_entries)) == NULL)
				return False;
			if ((q_r->uni_name = PRS_ALLOC_MEM(ps, UNISTR2, q_r->num_entries)) == NULL)
				return False;
		}
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unihdr("hdr_name", &q_r->hdr_name[i], ps, depth)) /* pointer names */
			return False;
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unistr2("dom_name", &q_r->uni_name[i], q_r->hdr_name[i].buffer, ps, depth)) /* names to be looked up */
			return False;
	}

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("num_trans_entries ", ps, depth, &q_r->num_trans_entries))
		return False;
	if(!prs_uint32("ptr_trans_sids ", ps, depth, &q_r->ptr_trans_sids))
		return False;
	if(!prs_uint16("lookup_level   ", ps, depth, &q_r->lookup_level))
		return False;
	if(!prs_align(ps))
		return False;
	if(!prs_uint32("mapped_count   ", ps, depth, &q_r->mapped_count))
		return False;
	if(!prs_uint32("unknown1   ", ps, depth, &q_r->unknown1))
		return False;
	if(!prs_uint32("unknown2   ", ps, depth, &q_r->unknown2))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/

bool lsa_io_r_lookup_names2(const char *desc, LSA_R_LOOKUP_NAMES2 *out, prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_r_lookup_names2");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("ptr_dom_ref", ps, depth, &out->ptr_dom_ref))
		return False;

	if (out->ptr_dom_ref != 0)
		if(!lsa_io_dom_r_ref("", out->dom_ref, ps, depth))
			return False;

	if(!prs_uint32("num_entries", ps, depth, &out->num_entries))
		return False;
	if(!prs_uint32("ptr_entries", ps, depth, &out->ptr_entries))
		return False;

	if (out->ptr_entries != 0) {
		if(!prs_uint32("num_entries2", ps, depth, &out->num_entries2))
			return False;

		if (out->num_entries2 != out->num_entries) {
			/* RPC fault */
			return False;
		}

		if (UNMARSHALLING(ps) && out->num_entries2) {
			if ((out->dom_rid = PRS_ALLOC_MEM(ps, DOM_RID2, out->num_entries2))
			    == NULL) {
				DEBUG(3, ("lsa_io_r_lookup_names2(): out of memory\n"));
				return False;
			}
		}

		for (i = 0; i < out->num_entries2; i++)
			if(!smb_io_dom_rid2("", &out->dom_rid[i], ps, depth)) /* domain RIDs being looked up */
				return False;
	}

	if(!prs_uint32("mapped_count", ps, depth, &out->mapped_count))
		return False;

	if(!prs_ntstatus("status      ", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Internal lsa data type io.
 Following pass must read DOM_SID2 types.
********************************************************************/

bool smb_io_lsa_translated_sids3(const char *desc, LSA_TRANSLATED_SID3 *q_r, 
			   prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "smb_io_lsa_translated_sids3");
	depth++;

	if(!prs_align(ps))
		return False;
	if(!prs_uint8 ("sid_type ", ps, depth, &q_r->sid_type ))
		return False;
	if(!prs_align(ps))
		return False;
	/* Second pass will read/write these. */
	if (!smb_io_dom_sid2_p("sid_header", ps, depth, &q_r->sid2))
		return False;
	if(!prs_uint32("sid_idx ", ps, depth, &q_r->sid_idx ))
		return False;
	if(!prs_uint32("unknown ", ps, depth, &q_r->unknown ))
		return False;
	
	return True;
}

/*******************************************************************
 Identical to lsa_io_q_lookup_names2.
********************************************************************/

bool lsa_io_q_lookup_names3(const char *desc, LSA_Q_LOOKUP_NAMES3 *q_r, 
			   prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_q_lookup_names3");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!smb_io_pol_hnd("", &q_r->pol, ps, depth)) /* policy handle */
		return False;

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("num_entries    ", ps, depth, &q_r->num_entries))
		return False;
	if(!prs_uint32("num_entries2   ", ps, depth, &q_r->num_entries2))
		return False;

	if (UNMARSHALLING(ps)) {
		if (q_r->num_entries) {
			if ((q_r->hdr_name = PRS_ALLOC_MEM(ps, UNIHDR, q_r->num_entries)) == NULL)
				return False;
			if ((q_r->uni_name = PRS_ALLOC_MEM(ps, UNISTR2, q_r->num_entries)) == NULL)
				return False;
		}
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unihdr("hdr_name", &q_r->hdr_name[i], ps, depth)) /* pointer names */
			return False;
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unistr2("dom_name", &q_r->uni_name[i], q_r->hdr_name[i].buffer, ps, depth)) /* names to be looked up */
			return False;
	}

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("num_trans_entries ", ps, depth, &q_r->num_trans_entries))
		return False;
	if(!prs_uint32("ptr_trans_sids ", ps, depth, &q_r->ptr_trans_sids))
		return False;
	if(!prs_uint16("lookup_level   ", ps, depth, &q_r->lookup_level))
		return False;
	if(!prs_align(ps))
		return False;
	if(!prs_uint32("mapped_count   ", ps, depth, &q_r->mapped_count))
		return False;
	if(!prs_uint32("unknown1   ", ps, depth, &q_r->unknown1))
		return False;
	if(!prs_uint32("unknown2   ", ps, depth, &q_r->unknown2))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/

bool lsa_io_r_lookup_names3(const char *desc, LSA_R_LOOKUP_NAMES3 *out, prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_r_lookup_names3");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("ptr_dom_ref", ps, depth, &out->ptr_dom_ref))
		return False;

	if (out->ptr_dom_ref != 0)
		if(!lsa_io_dom_r_ref("", out->dom_ref, ps, depth))
			return False;

	if(!prs_uint32("num_entries", ps, depth, &out->num_entries))
		return False;
	if(!prs_uint32("ptr_entries", ps, depth, &out->ptr_entries))
		return False;

	if (out->ptr_entries != 0) {
		if(!prs_uint32("num_entries2", ps, depth, &out->num_entries2))
			return False;

		if (out->num_entries2 != out->num_entries) {
			/* RPC fault */
			return False;
		}

		if (UNMARSHALLING(ps) && out->num_entries2) {
			if ((out->trans_sids = PRS_ALLOC_MEM(ps, LSA_TRANSLATED_SID3, out->num_entries2))
			    == NULL) {
				DEBUG(3, ("lsa_io_r_lookup_names3(): out of memory\n"));
				return False;
			}
		}

		for (i = 0; i < out->num_entries2; i++) {
			if(!smb_io_lsa_translated_sids3("", &out->trans_sids[i], ps, depth)) {
				return False;
			}
		}
		/* Now process the DOM_SID2 entries. */
		for (i = 0; i < out->num_entries2; i++) {
			if (out->trans_sids[i].sid2) {
				if( !smb_io_dom_sid2("sid2", out->trans_sids[i].sid2, ps, depth) ) {
					return False;
				}
			}
		}
	}

	if(!prs_uint32("mapped_count", ps, depth, &out->mapped_count))
		return False;

	if(!prs_ntstatus("status      ", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
********************************************************************/

bool lsa_io_q_lookup_names4(const char *desc, LSA_Q_LOOKUP_NAMES4 *q_r, 
			   prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_q_lookup_names4");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("num_entries    ", ps, depth, &q_r->num_entries))
		return False;
	if(!prs_uint32("num_entries2   ", ps, depth, &q_r->num_entries2))
		return False;

	if (UNMARSHALLING(ps)) {
		if (q_r->num_entries) {
			if ((q_r->hdr_name = PRS_ALLOC_MEM(ps, UNIHDR, q_r->num_entries)) == NULL)
				return False;
			if ((q_r->uni_name = PRS_ALLOC_MEM(ps, UNISTR2, q_r->num_entries)) == NULL)
				return False;
		}
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unihdr("hdr_name", &q_r->hdr_name[i], ps, depth)) /* pointer names */
			return False;
	}

	for (i = 0; i < q_r->num_entries; i++) {
		if(!prs_align(ps))
			return False;
		if(!smb_io_unistr2("dom_name", &q_r->uni_name[i], q_r->hdr_name[i].buffer, ps, depth)) /* names to be looked up */
			return False;
	}

	if(!prs_align(ps))
		return False;
	if(!prs_uint32("num_trans_entries ", ps, depth, &q_r->num_trans_entries))
		return False;
	if(!prs_uint32("ptr_trans_sids ", ps, depth, &q_r->ptr_trans_sids))
		return False;
	if(!prs_uint16("lookup_level   ", ps, depth, &q_r->lookup_level))
		return False;
	if(!prs_align(ps))
		return False;
	if(!prs_uint32("mapped_count   ", ps, depth, &q_r->mapped_count))
		return False;
	if(!prs_uint32("unknown1   ", ps, depth, &q_r->unknown1))
		return False;
	if(!prs_uint32("unknown2   ", ps, depth, &q_r->unknown2))
		return False;

	return True;
}

/*******************************************************************
 Identical to lsa_io_r_lookup_names3.
********************************************************************/

bool lsa_io_r_lookup_names4(const char *desc, LSA_R_LOOKUP_NAMES4 *out, prs_struct *ps, int depth)
{
	unsigned int i;

	prs_debug(ps, depth, desc, "lsa_io_r_lookup_names4");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("ptr_dom_ref", ps, depth, &out->ptr_dom_ref))
		return False;

	if (out->ptr_dom_ref != 0)
		if(!lsa_io_dom_r_ref("", out->dom_ref, ps, depth))
			return False;

	if(!prs_uint32("num_entries", ps, depth, &out->num_entries))
		return False;
	if(!prs_uint32("ptr_entries", ps, depth, &out->ptr_entries))
		return False;

	if (out->ptr_entries != 0) {
		if(!prs_uint32("num_entries2", ps, depth, &out->num_entries2))
			return False;

		if (out->num_entries2 != out->num_entries) {
			/* RPC fault */
			return False;
		}

		if (UNMARSHALLING(ps) && out->num_entries2) {
			if ((out->trans_sids = PRS_ALLOC_MEM(ps, LSA_TRANSLATED_SID3, out->num_entries2))
			    == NULL) {
				DEBUG(3, ("lsa_io_r_lookup_names4(): out of memory\n"));
				return False;
			}
		}

		for (i = 0; i < out->num_entries2; i++) {
			if(!smb_io_lsa_translated_sids3("", &out->trans_sids[i], ps, depth)) {
				return False;
			}
		}
		/* Now process the DOM_SID2 entries. */
		for (i = 0; i < out->num_entries2; i++) {
			if (out->trans_sids[i].sid2) {
				if( !smb_io_dom_sid2("sid2", out->trans_sids[i].sid2, ps, depth) ) {
					return False;
				}
			}
		}
	}

	if(!prs_uint32("mapped_count", ps, depth, &out->mapped_count))
		return False;

	if(!prs_ntstatus("status      ", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Inits an LSA_Q_ENUM_PRIVS structure.
********************************************************************/

void init_q_enum_privs(LSA_Q_ENUM_PRIVS *in, POLICY_HND *hnd, uint32 enum_context, uint32 pref_max_length)
{
	DEBUG(5, ("init_q_enum_privs\n"));

	memcpy(&in->pol, hnd, sizeof(in->pol));

	in->enum_context = enum_context;
	in->pref_max_length = pref_max_length;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/
bool lsa_io_q_enum_privs(const char *desc, LSA_Q_ENUM_PRIVS *in, prs_struct *ps, int depth)
{
	if (in == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_q_enum_privs");
	depth++;

	if (!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if(!prs_uint32("enum_context   ", ps, depth, &in->enum_context))
		return False;
	if(!prs_uint32("pref_max_length", ps, depth, &in->pref_max_length))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/
static bool lsa_io_priv_entries(const char *desc, LSA_PRIV_ENTRY *entries, uint32 count, prs_struct *ps, int depth)
{
	uint32 i;

	if (entries == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_priv_entries");
	depth++;

	if(!prs_align(ps))
		return False;

	for (i = 0; i < count; i++) {
		if (!smb_io_unihdr("", &entries[i].hdr_name, ps, depth))
			return False;
		if(!prs_uint32("luid_low ", ps, depth, &entries[i].luid_low))
			return False;
		if(!prs_uint32("luid_high", ps, depth, &entries[i].luid_high))
			return False;
	}

	for (i = 0; i < count; i++)
		if (!smb_io_unistr2("", &entries[i].name, entries[i].hdr_name.buffer, ps, depth))
			return False;

	return True;
}

/*******************************************************************
 Inits an LSA_R_ENUM_PRIVS structure.
********************************************************************/

void init_lsa_r_enum_privs(LSA_R_ENUM_PRIVS *out, uint32 enum_context,
			  uint32 count, LSA_PRIV_ENTRY *entries)
{
	DEBUG(5, ("init_lsa_r_enum_privs\n"));

	out->enum_context=enum_context;
	out->count=count;
	
	if (entries!=NULL) {
		out->ptr=1;
		out->count1=count;
		out->privs=entries;
	} else {
		out->ptr=0;
		out->count1=0;
		out->privs=NULL;
	}		
}

/*******************************************************************
reads or writes a structure.
********************************************************************/
bool lsa_io_r_enum_privs(const char *desc, LSA_R_ENUM_PRIVS *out, prs_struct *ps, int depth)
{
	if (out == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_r_enum_privs");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("enum_context", ps, depth, &out->enum_context))
		return False;
	if(!prs_uint32("count", ps, depth, &out->count))
		return False;
	if(!prs_uint32("ptr", ps, depth, &out->ptr))
		return False;

	if (out->ptr) {
		if(!prs_uint32("count1", ps, depth, &out->count1))
			return False;

		if (UNMARSHALLING(ps) && out->count1)
			if (!(out->privs = PRS_ALLOC_MEM(ps, LSA_PRIV_ENTRY, out->count1)))
				return False;

		if (!lsa_io_priv_entries("", out->privs, out->count1, ps, depth))
			return False;
	}

	if(!prs_align(ps))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

void init_lsa_priv_get_dispname(LSA_Q_PRIV_GET_DISPNAME *trn, POLICY_HND *hnd, const char *name, uint16 lang_id, uint16 lang_id_sys)
{
	memcpy(&trn->pol, hnd, sizeof(trn->pol));

	init_unistr2(&trn->name, name, UNI_FLAGS_NONE);
	init_uni_hdr(&trn->hdr_name, &trn->name);
	trn->lang_id = lang_id;
	trn->lang_id_sys = lang_id_sys;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/
bool lsa_io_q_priv_get_dispname(const char *desc, LSA_Q_PRIV_GET_DISPNAME *in, prs_struct *ps, int depth)
{
	if (in == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_q_priv_get_dispname");
	depth++;

	if(!prs_align(ps))
		return False;

	if (!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if (!smb_io_unihdr("hdr_name", &in->hdr_name, ps, depth))
		return False;

	if (!smb_io_unistr2("name", &in->name, in->hdr_name.buffer, ps, depth))
		return False;

	if(!prs_uint16("lang_id    ", ps, depth, &in->lang_id))
		return False;
	if(!prs_uint16("lang_id_sys", ps, depth, &in->lang_id_sys))
		return False;

	return True;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/
bool lsa_io_r_priv_get_dispname(const char *desc, LSA_R_PRIV_GET_DISPNAME *out, prs_struct *ps, int depth)
{
	if (out == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_r_priv_get_dispname");
	depth++;

	if (!prs_align(ps))
		return False;

	if (!prs_uint32("ptr_info", ps, depth, &out->ptr_info))
		return False;

	if (out->ptr_info){
		if (!smb_io_unihdr("hdr_name", &out->hdr_desc, ps, depth))
			return False;

		if (!smb_io_unistr2("desc", &out->desc, out->hdr_desc.buffer, ps, depth))
			return False;
	}
/*
	if(!prs_align(ps))
		return False;
*/
	if(!prs_uint16("lang_id", ps, depth, &out->lang_id))
		return False;

	if(!prs_align(ps))
		return False;
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*
  initialise a LSA_Q_ENUM_ACCOUNTS structure
*/
void init_lsa_q_enum_accounts(LSA_Q_ENUM_ACCOUNTS *trn, POLICY_HND *hnd, uint32 enum_context, uint32 pref_max_length)
{
	memcpy(&trn->pol, hnd, sizeof(trn->pol));

	trn->enum_context = enum_context;
	trn->pref_max_length = pref_max_length;
}

/*******************************************************************
reads or writes a structure.
********************************************************************/
bool lsa_io_q_enum_accounts(const char *desc, LSA_Q_ENUM_ACCOUNTS *in, prs_struct *ps, int depth)
{
	if (in == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_q_enum_accounts");
	depth++;

	if (!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if(!prs_uint32("enum_context   ", ps, depth, &in->enum_context))
		return False;
	if(!prs_uint32("pref_max_length", ps, depth, &in->pref_max_length))
		return False;

	return True;
}


/*******************************************************************
 Inits an LSA_R_ENUM_PRIVS structure.
********************************************************************/

void init_lsa_r_enum_accounts(LSA_R_ENUM_ACCOUNTS *out, uint32 enum_context)
{
	DEBUG(5, ("init_lsa_r_enum_accounts\n"));

	out->enum_context=enum_context;
	if (out->enum_context!=0) {
		out->sids.num_entries=enum_context;
		out->sids.ptr_sid_enum=1;
		out->sids.num_entries2=enum_context;
	} else {
		out->sids.num_entries=0;
		out->sids.ptr_sid_enum=0;
		out->sids.num_entries2=0;
	}
}

/*******************************************************************
reads or writes a structure.
********************************************************************/
bool lsa_io_r_enum_accounts(const char *desc, LSA_R_ENUM_ACCOUNTS *out, prs_struct *ps, int depth)
{
	if (out == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_r_enum_accounts");
	depth++;

	if (!prs_align(ps))
		return False;

	if(!prs_uint32("enum_context", ps, depth, &out->enum_context))
		return False;

	if (!lsa_io_sid_enum("sids", &out->sids, ps, depth))
		return False;

	if (!prs_align(ps))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}


/*******************************************************************
 Reads or writes an LSA_Q_UNK_GET_CONNUSER structure.
********************************************************************/

bool lsa_io_q_unk_get_connuser(const char *desc, LSA_Q_UNK_GET_CONNUSER *in, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_unk_get_connuser");
	depth++;

	if(!prs_align(ps))
		return False;
   
	if(!prs_uint32("ptr_srvname", ps, depth, &in->ptr_srvname))
		return False;

	if(!smb_io_unistr2("uni2_srvname", &in->uni2_srvname, in->ptr_srvname, ps, depth)) /* server name to be looked up */
		return False;

	if (!prs_align(ps))
	  return False;

	if(!prs_uint32("unk1", ps, depth, &in->unk1))
		return False;
	if(!prs_uint32("unk2", ps, depth, &in->unk2))
		return False;
	if(!prs_uint32("unk3", ps, depth, &in->unk3))
		return False;

	/* Don't bother to read or write at present... */
	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_UNK_GET_CONNUSER structure.
********************************************************************/

bool lsa_io_r_unk_get_connuser(const char *desc, LSA_R_UNK_GET_CONNUSER *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_unk_get_connuser");
	depth++;

	if(!prs_align(ps))
		return False;
   
	if(!prs_uint32("ptr_user_name", ps, depth, &out->ptr_user_name))
		return False;
	if(!smb_io_unihdr("hdr_user_name", &out->hdr_user_name, ps, depth))
		return False;
	if(!smb_io_unistr2("uni2_user_name", &out->uni2_user_name, out->ptr_user_name, ps, depth))
		return False;

	if (!prs_align(ps))
	  return False;
	
	if(!prs_uint32("unk1", ps, depth, &out->unk1))
		return False;

	if(!prs_uint32("ptr_dom_name", ps, depth, &out->ptr_dom_name))
		return False;
	if(!smb_io_unihdr("hdr_dom_name", &out->hdr_dom_name, ps, depth))
		return False;
	if(!smb_io_unistr2("uni2_dom_name", &out->uni2_dom_name, out->ptr_dom_name, ps, depth))
		return False;

	if (!prs_align(ps))
	  return False;
	
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

void init_lsa_q_create_account(LSA_Q_CREATEACCOUNT *trn, POLICY_HND *hnd, DOM_SID *sid, uint32 desired_access)
{
	memcpy(&trn->pol, hnd, sizeof(trn->pol));

	init_dom_sid2(&trn->sid, sid);
	trn->access = desired_access;
}


/*******************************************************************
 Reads or writes an LSA_Q_CREATEACCOUNT structure.
********************************************************************/

bool lsa_io_q_create_account(const char *desc, LSA_Q_CREATEACCOUNT *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_create_account");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;

	if(!smb_io_dom_sid2("sid", &out->sid, ps, depth)) /* domain SID */
		return False;

 	if(!prs_uint32("access", ps, depth, &out->access))
		return False;
  
	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_CREATEACCOUNT structure.
********************************************************************/

bool lsa_io_r_create_account(const char *desc, LSA_R_CREATEACCOUNT  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_open_account");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}


void init_lsa_q_open_account(LSA_Q_OPENACCOUNT *trn, POLICY_HND *hnd, DOM_SID *sid, uint32 desired_access)
{
	memcpy(&trn->pol, hnd, sizeof(trn->pol));

	init_dom_sid2(&trn->sid, sid);
	trn->access = desired_access;
}

/*******************************************************************
 Reads or writes an LSA_Q_OPENACCOUNT structure.
********************************************************************/

bool lsa_io_q_open_account(const char *desc, LSA_Q_OPENACCOUNT *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_open_account");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;

	if(!smb_io_dom_sid2("sid", &out->sid, ps, depth)) /* domain SID */
		return False;

 	if(!prs_uint32("access", ps, depth, &out->access))
		return False;
  
	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_OPENACCOUNT structure.
********************************************************************/

bool lsa_io_r_open_account(const char *desc, LSA_R_OPENACCOUNT  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_open_account");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}


void init_lsa_q_enum_privsaccount(LSA_Q_ENUMPRIVSACCOUNT *trn, POLICY_HND *hnd)
{
	memcpy(&trn->pol, hnd, sizeof(trn->pol));

}

/*******************************************************************
 Reads or writes an LSA_Q_ENUMPRIVSACCOUNT structure.
********************************************************************/

bool lsa_io_q_enum_privsaccount(const char *desc, LSA_Q_ENUMPRIVSACCOUNT *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_enum_privsaccount");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LUID structure.
********************************************************************/

static bool lsa_io_luid(const char *desc, LUID *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_luid");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!prs_uint32("low", ps, depth, &out->low))
		return False;

	if(!prs_uint32("high", ps, depth, &out->high))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LUID_ATTR structure.
********************************************************************/

static bool lsa_io_luid_attr(const char *desc, LUID_ATTR *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_luid_attr");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if (!lsa_io_luid(desc, &out->luid, ps, depth))
		return False;

	if(!prs_uint32("attr", ps, depth, &out->attr))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an PRIVILEGE_SET structure.
********************************************************************/

static bool lsa_io_privilege_set(const char *desc, PRIVILEGE_SET *out, prs_struct *ps, int depth)
{
	uint32 i, dummy;

	prs_debug(ps, depth, desc, "lsa_io_privilege_set");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!prs_uint32("count", ps, depth, &dummy))
		return False;
	if(!prs_uint32("control", ps, depth, &out->control))
		return False;

	for (i=0; i<out->count; i++) {
		if (!lsa_io_luid_attr(desc, &out->set[i], ps, depth))
			return False;
	}
	
	return True;
}

NTSTATUS init_lsa_r_enum_privsaccount(TALLOC_CTX *mem_ctx, LSA_R_ENUMPRIVSACCOUNT *out, LUID_ATTR *set, uint32 count, uint32 control)
{
	NTSTATUS ret = NT_STATUS_OK;

	out->ptr = 1;
	out->count = count;

	if ( !NT_STATUS_IS_OK(ret = privilege_set_init_by_ctx(mem_ctx, &(out->set))) )
		return ret;
	
	out->set.count = count;
	
	if (!NT_STATUS_IS_OK(ret = dup_luid_attr(out->set.mem_ctx, &(out->set.set), set, count)))
		return ret;

	DEBUG(10,("init_lsa_r_enum_privsaccount: %d privileges\n", out->count));

	return ret;
}

/*******************************************************************
 Reads or writes an LSA_R_ENUMPRIVSACCOUNT structure.
********************************************************************/

bool lsa_io_r_enum_privsaccount(const char *desc, LSA_R_ENUMPRIVSACCOUNT *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_enum_privsaccount");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!prs_uint32("ptr", ps, depth, &out->ptr))
		return False;

	if (out->ptr!=0) {
		if(!prs_uint32("count", ps, depth, &out->count))
			return False;

		/* malloc memory if unmarshalling here */

		if (UNMARSHALLING(ps) && out->count != 0) {
			if (!NT_STATUS_IS_OK(privilege_set_init_by_ctx(ps->mem_ctx, &(out->set))))
				return False;

			if (!(out->set.set = PRS_ALLOC_MEM(ps,LUID_ATTR,out->count)))
				return False;

		}
		
		if(!lsa_io_privilege_set(desc, &out->set, ps, depth))
			return False;
	}

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}



/*******************************************************************
 Reads or writes an  LSA_Q_GETSYSTEMACCOUNTstructure.
********************************************************************/

bool lsa_io_q_getsystemaccount(const char *desc, LSA_Q_GETSYSTEMACCOUNT  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_getsystemaccount");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an  LSA_R_GETSYSTEMACCOUNTstructure.
********************************************************************/

bool lsa_io_r_getsystemaccount(const char *desc, LSA_R_GETSYSTEMACCOUNT  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_getsystemaccount");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!prs_uint32("access", ps, depth, &out->access))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}


/*******************************************************************
 Reads or writes an LSA_Q_SETSYSTEMACCOUNT structure.
********************************************************************/

bool lsa_io_q_setsystemaccount(const char *desc, LSA_Q_SETSYSTEMACCOUNT  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_setsystemaccount");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;

	if(!prs_uint32("access", ps, depth, &out->access))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_SETSYSTEMACCOUNT structure.
********************************************************************/

bool lsa_io_r_setsystemaccount(const char *desc, LSA_R_SETSYSTEMACCOUNT  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_setsystemaccount");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}


void init_lsa_string( LSA_STRING *uni, const char *string )
{
	init_unistr2(&uni->unistring, string, UNI_FLAGS_NONE);
	init_uni_hdr(&uni->hdr, &uni->unistring);
}

void init_lsa_q_lookup_priv_value(LSA_Q_LOOKUP_PRIV_VALUE *q_u, POLICY_HND *hnd, const char *name)
{
	memcpy(&q_u->pol, hnd, sizeof(q_u->pol));
	init_lsa_string( &q_u->privname, name );
}

bool smb_io_lsa_string( const char *desc, LSA_STRING *string, prs_struct *ps, int depth )
{
	prs_debug(ps, depth, desc, "smb_io_lsa_string");
	depth++;

	if(!smb_io_unihdr ("hdr", &string->hdr, ps, depth))
		return False;
	if(!smb_io_unistr2("unistring", &string->unistring, string->hdr.buffer, ps, depth))
		return False;
	
	return True;
}

/*******************************************************************
 Reads or writes an LSA_Q_LOOKUP_PRIV_VALUE  structure.
********************************************************************/

bool lsa_io_q_lookup_priv_value(const char *desc, LSA_Q_LOOKUP_PRIV_VALUE  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_lookup_priv_value");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;
	if(!smb_io_lsa_string("privname", &out->privname, ps, depth))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an  LSA_R_LOOKUP_PRIV_VALUE structure.
********************************************************************/

bool lsa_io_r_lookup_priv_value(const char *desc, LSA_R_LOOKUP_PRIV_VALUE  *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_lookup_priv_value");
	depth++;

	if(!prs_align(ps))
		return False;
		
	if(!lsa_io_luid("luid", &out->luid, ps, depth))
		return False;
 
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}


/*******************************************************************
 Reads or writes an LSA_Q_ADDPRIVS structure.
********************************************************************/

bool lsa_io_q_addprivs(const char *desc, LSA_Q_ADDPRIVS *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_addprivs");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;
	
	if(!prs_uint32("count", ps, depth, &out->count))
		return False;

	if (UNMARSHALLING(ps) && out->count!=0) {
		if (!NT_STATUS_IS_OK(privilege_set_init_by_ctx(ps->mem_ctx, &(out->set))))
			return False;
		
		if (!(out->set.set = PRS_ALLOC_MEM(ps, LUID_ATTR, out->count)))
			return False;
	}
	
	if(!lsa_io_privilege_set(desc, &out->set, ps, depth))
		return False;
	
	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_ADDPRIVS structure.
********************************************************************/

bool lsa_io_r_addprivs(const char *desc, LSA_R_ADDPRIVS *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_addprivs");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LSA_Q_REMOVEPRIVS structure.
********************************************************************/

bool lsa_io_q_removeprivs(const char *desc, LSA_Q_REMOVEPRIVS *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_removeprivs");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &out->pol, ps, depth))
		return False;
	
	if(!prs_uint32("allrights", ps, depth, &out->allrights))
		return False;

	if(!prs_uint32("ptr", ps, depth, &out->ptr))
		return False;

	/* 
	 * JFM: I'm not sure at all if the count is inside the ptr
	 * never seen one with ptr=0
	 */

	if (out->ptr!=0) {
		if(!prs_uint32("count", ps, depth, &out->count))
			return False;

		if (UNMARSHALLING(ps) && out->count!=0) {
			if (!NT_STATUS_IS_OK(privilege_set_init_by_ctx(ps->mem_ctx, &(out->set))))
				return False;

			if (!(out->set.set = PRS_ALLOC_MEM(ps, LUID_ATTR, out->count)))
				return False;
		}

		if(!lsa_io_privilege_set(desc, &out->set, ps, depth))
			return False;
	}

	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_REMOVEPRIVS structure.
********************************************************************/

bool lsa_io_r_removeprivs(const char *desc, LSA_R_REMOVEPRIVS *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_removeprivs");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

bool policy_handle_is_valid(const POLICY_HND *hnd)
{
	POLICY_HND zero_pol;

	ZERO_STRUCT(zero_pol);
	return ((memcmp(&zero_pol, hnd, sizeof(POLICY_HND)) == 0) ? False : True );
}

/*******************************************************************
 Inits an LSA_Q_QUERY_INFO2 structure.
********************************************************************/

void init_q_query2(LSA_Q_QUERY_INFO2 *in, POLICY_HND *hnd, uint16 info_class)
{
	DEBUG(5, ("init_q_query2\n"));

	memcpy(&in->pol, hnd, sizeof(in->pol));

	in->info_class = info_class;
}

/*******************************************************************
 Reads or writes an LSA_Q_QUERY_DNSDOMINFO structure.
********************************************************************/

bool lsa_io_q_query_info2(const char *desc, LSA_Q_QUERY_INFO2 *in, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_query_info2");
	depth++;

	if(!prs_align(ps))
		return False;
 
	if(!smb_io_pol_hnd("pol", &in->pol, ps, depth))
		return False;
	
	if(!prs_uint16("info_class", ps, depth, &in->info_class))
		return False;

	return True;
}

/*******************************************************************
 Reads or writes an LSA_R_QUERY_DNSDOMINFO structure.
********************************************************************/

bool lsa_io_r_query_info2(const char *desc, LSA_R_QUERY_INFO2 *out,
			  prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_query_info2");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!prs_uint32("dom_ptr", ps, depth, &out->dom_ptr))
		return False;

	if (out->dom_ptr) {

		if(!lsa_io_query_info_ctr2("", ps, depth, &out->ctr))
			return False;
	}

	if(!prs_align(ps))
		return False;
	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Inits an LSA_Q_ENUM_ACCT_RIGHTS structure.
********************************************************************/
void init_q_enum_acct_rights(LSA_Q_ENUM_ACCT_RIGHTS *in, 
			     POLICY_HND *hnd, 
			     uint32 count, 
			     DOM_SID *sid)
{
	DEBUG(5, ("init_q_enum_acct_rights\n"));

	in->pol = *hnd;
	init_dom_sid2(&in->sid, sid);
}

/*******************************************************************
********************************************************************/
NTSTATUS init_r_enum_acct_rights( LSA_R_ENUM_ACCT_RIGHTS *out, PRIVILEGE_SET *privileges )
{
	uint32 i;
	const char *privname;
	const char **privname_array = NULL;
	int num_priv = 0;

	for ( i=0; i<privileges->count; i++ ) {
		privname = luid_to_privilege_name( &privileges->set[i].luid );
		if ( privname ) {
			if ( !add_string_to_array( talloc_tos(), privname, &privname_array, &num_priv ) )
				return NT_STATUS_NO_MEMORY;
		}
	}

	if ( num_priv ) {
		out->rights = TALLOC_P( talloc_tos(), UNISTR4_ARRAY );
		if (!out->rights) {
			return NT_STATUS_NO_MEMORY;
		}

		if ( !init_unistr4_array( out->rights, num_priv, privname_array ) ) 
			return NT_STATUS_NO_MEMORY;

		out->count = num_priv;
	}

	return NT_STATUS_OK;
}

/*******************************************************************
reads or writes a LSA_Q_ENUM_ACCT_RIGHTS structure.
********************************************************************/
bool lsa_io_q_enum_acct_rights(const char *desc, LSA_Q_ENUM_ACCT_RIGHTS *in, prs_struct *ps, int depth)
{
	
	if (in == NULL)
		return False;

	prs_debug(ps, depth, desc, "lsa_io_q_enum_acct_rights");
	depth++;

	if (!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if(!smb_io_dom_sid2("sid", &in->sid, ps, depth))
		return False;

	return True;
}


/*******************************************************************
reads or writes a LSA_R_ENUM_ACCT_RIGHTS structure.
********************************************************************/
bool lsa_io_r_enum_acct_rights(const char *desc, LSA_R_ENUM_ACCT_RIGHTS *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_enum_acct_rights");
	depth++;

	if(!prs_uint32("count   ", ps, depth, &out->count))
		return False;

	if ( !prs_pointer("rights", ps, depth, (void*)&out->rights, sizeof(UNISTR4_ARRAY), (PRS_POINTER_CAST)prs_unistr4_array) )
		return False;

	if(!prs_align(ps))
		return False;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}


/*******************************************************************
 Inits an LSA_Q_ADD_ACCT_RIGHTS structure.
********************************************************************/
void init_q_add_acct_rights( LSA_Q_ADD_ACCT_RIGHTS *in, POLICY_HND *hnd, 
                             DOM_SID *sid, uint32 count, const char **rights )
{
	DEBUG(5, ("init_q_add_acct_rights\n"));

	in->pol = *hnd;
	init_dom_sid2(&in->sid, sid);
	
	in->rights = TALLOC_P( talloc_tos(), UNISTR4_ARRAY );
	if (!in->rights) {
		smb_panic("init_q_add_acct_rights: talloc fail\n");
		return;
	}
	init_unistr4_array( in->rights, count, rights );
	
	in->count = count;
}


/*******************************************************************
reads or writes a LSA_Q_ADD_ACCT_RIGHTS structure.
********************************************************************/
bool lsa_io_q_add_acct_rights(const char *desc, LSA_Q_ADD_ACCT_RIGHTS *in, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_add_acct_rights");
	depth++;

	if (!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if(!smb_io_dom_sid2("sid", &in->sid, ps, depth))
		return False;

	if(!prs_uint32("count", ps, depth, &in->count))
		return False;

	if ( !prs_pointer("rights", ps, depth, (void*)&in->rights, sizeof(UNISTR4_ARRAY), (PRS_POINTER_CAST)prs_unistr4_array) )
		return False;

	return True;
}

/*******************************************************************
reads or writes a LSA_R_ENUM_ACCT_RIGHTS structure.
********************************************************************/
bool lsa_io_r_add_acct_rights(const char *desc, LSA_R_ADD_ACCT_RIGHTS *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_add_acct_rights");
	depth++;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
 Inits an LSA_Q_REMOVE_ACCT_RIGHTS structure.
********************************************************************/

void init_q_remove_acct_rights(LSA_Q_REMOVE_ACCT_RIGHTS *in, 
			       POLICY_HND *hnd, 
			       DOM_SID *sid,
			       uint32 removeall,
			       uint32 count, 
			       const char **rights)
{
	DEBUG(5, ("init_q_remove_acct_rights\n"));

	in->pol = *hnd;

	init_dom_sid2(&in->sid, sid);

	in->removeall = removeall;
	in->count = count;

	in->rights = TALLOC_P( talloc_tos(), UNISTR4_ARRAY );
	if (!in->rights) {
		smb_panic("init_q_remove_acct_rights: talloc fail\n");
		return;
	}
	init_unistr4_array( in->rights, count, rights );
}

/*******************************************************************
reads or writes a LSA_Q_REMOVE_ACCT_RIGHTS structure.
********************************************************************/

bool lsa_io_q_remove_acct_rights(const char *desc, LSA_Q_REMOVE_ACCT_RIGHTS *in, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_remove_acct_rights");
	depth++;

	if (!smb_io_pol_hnd("", &in->pol, ps, depth))
		return False;

	if(!smb_io_dom_sid2("sid", &in->sid, ps, depth))
		return False;

	if(!prs_uint32("removeall", ps, depth, &in->removeall))
		return False;

	if(!prs_uint32("count", ps, depth, &in->count))
		return False;

	if ( !prs_pointer("rights", ps, depth, (void*)&in->rights, sizeof(UNISTR4_ARRAY), (PRS_POINTER_CAST)prs_unistr4_array) )
		return False;

	return True;
}

/*******************************************************************
reads or writes a LSA_R_ENUM_ACCT_RIGHTS structure.
********************************************************************/
bool lsa_io_r_remove_acct_rights(const char *desc, LSA_R_REMOVE_ACCT_RIGHTS *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_remove_acct_rights");
	depth++;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}

/*******************************************************************
********************************************************************/

bool lsa_io_q_delete_object(const char *desc, LSA_Q_DELETE_OBJECT *in, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_q_delete_object");
	depth++;

	if(!prs_align(ps))
		return False;

	if(!smb_io_pol_hnd("", &in->handle, ps, depth))
		return False;

	return True;
}

/*******************************************************************
********************************************************************/

bool lsa_io_r_delete_object(const char *desc, LSA_R_DELETE_OBJECT *out, prs_struct *ps, int depth)
{
	prs_debug(ps, depth, desc, "lsa_io_r_delete_object");
	depth++;

	if(!prs_ntstatus("status", ps, depth, &out->status))
		return False;

	return True;
}
