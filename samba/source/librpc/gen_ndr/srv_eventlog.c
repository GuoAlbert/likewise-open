/*
 * Unix SMB/CIFS implementation.
 * server auto-generated by pidl. DO NOT MODIFY!
 */

#include "includes.h"
#include "librpc/gen_ndr/srv_eventlog.h"

static bool api_eventlog_ClearEventLogW(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_ClearEventLogW *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_CLEAREVENTLOGW];

	r = talloc(NULL, struct eventlog_ClearEventLogW);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_ClearEventLogW, r);
	}

	r->out.result = _eventlog_ClearEventLogW(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_ClearEventLogW, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_BackupEventLogW(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_BackupEventLogW *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_BACKUPEVENTLOGW];

	r = talloc(NULL, struct eventlog_BackupEventLogW);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_BackupEventLogW, r);
	}

	r->out.result = _eventlog_BackupEventLogW(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_BackupEventLogW, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_CloseEventLog(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_CloseEventLog *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_CLOSEEVENTLOG];

	r = talloc(NULL, struct eventlog_CloseEventLog);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_CloseEventLog, r);
	}

	ZERO_STRUCT(r->out);
	r->out.handle = r->in.handle;
	r->out.result = _eventlog_CloseEventLog(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_CloseEventLog, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_DeregisterEventSource(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_DeregisterEventSource *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_DEREGISTEREVENTSOURCE];

	r = talloc(NULL, struct eventlog_DeregisterEventSource);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_DeregisterEventSource, r);
	}

	r->out.result = _eventlog_DeregisterEventSource(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_DeregisterEventSource, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_GetNumRecords(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_GetNumRecords *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_GETNUMRECORDS];

	r = talloc(NULL, struct eventlog_GetNumRecords);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_GetNumRecords, r);
	}

	ZERO_STRUCT(r->out);
	r->out.number = talloc_zero(r, uint32_t);
	if (r->out.number == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _eventlog_GetNumRecords(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_GetNumRecords, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_GetOldestRecord(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_GetOldestRecord *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_GETOLDESTRECORD];

	r = talloc(NULL, struct eventlog_GetOldestRecord);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_GetOldestRecord, r);
	}

	ZERO_STRUCT(r->out);
	r->out.oldest_entry = talloc_zero(r, uint32_t);
	if (r->out.oldest_entry == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _eventlog_GetOldestRecord(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_GetOldestRecord, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_ChangeNotify(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_ChangeNotify *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_CHANGENOTIFY];

	r = talloc(NULL, struct eventlog_ChangeNotify);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_ChangeNotify, r);
	}

	r->out.result = _eventlog_ChangeNotify(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_ChangeNotify, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_OpenEventLogW(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_OpenEventLogW *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_OPENEVENTLOGW];

	r = talloc(NULL, struct eventlog_OpenEventLogW);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_OpenEventLogW, r);
	}

	ZERO_STRUCT(r->out);
	r->out.handle = talloc_zero(r, struct policy_handle);
	if (r->out.handle == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _eventlog_OpenEventLogW(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_OpenEventLogW, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_RegisterEventSourceW(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_RegisterEventSourceW *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_REGISTEREVENTSOURCEW];

	r = talloc(NULL, struct eventlog_RegisterEventSourceW);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_RegisterEventSourceW, r);
	}

	r->out.result = _eventlog_RegisterEventSourceW(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_RegisterEventSourceW, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_OpenBackupEventLogW(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_OpenBackupEventLogW *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_OPENBACKUPEVENTLOGW];

	r = talloc(NULL, struct eventlog_OpenBackupEventLogW);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_OpenBackupEventLogW, r);
	}

	r->out.result = _eventlog_OpenBackupEventLogW(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_OpenBackupEventLogW, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_ReadEventLogW(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_ReadEventLogW *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_READEVENTLOGW];

	r = talloc(NULL, struct eventlog_ReadEventLogW);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_ReadEventLogW, r);
	}

	ZERO_STRUCT(r->out);
	r->out.data = talloc_zero_array(r, uint8_t, r->in.number_of_bytes);
	if (r->out.data == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.sent_size = talloc_zero(r, uint32_t);
	if (r->out.sent_size == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.real_size = talloc_zero(r, uint32_t);
	if (r->out.real_size == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _eventlog_ReadEventLogW(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_ReadEventLogW, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_ReportEventW(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_ReportEventW *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_REPORTEVENTW];

	r = talloc(NULL, struct eventlog_ReportEventW);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_ReportEventW, r);
	}

	r->out.result = _eventlog_ReportEventW(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_ReportEventW, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_ClearEventLogA(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_ClearEventLogA *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_CLEAREVENTLOGA];

	r = talloc(NULL, struct eventlog_ClearEventLogA);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_ClearEventLogA, r);
	}

	r->out.result = _eventlog_ClearEventLogA(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_ClearEventLogA, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_BackupEventLogA(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_BackupEventLogA *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_BACKUPEVENTLOGA];

	r = talloc(NULL, struct eventlog_BackupEventLogA);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_BackupEventLogA, r);
	}

	r->out.result = _eventlog_BackupEventLogA(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_BackupEventLogA, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_OpenEventLogA(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_OpenEventLogA *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_OPENEVENTLOGA];

	r = talloc(NULL, struct eventlog_OpenEventLogA);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_OpenEventLogA, r);
	}

	r->out.result = _eventlog_OpenEventLogA(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_OpenEventLogA, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_RegisterEventSourceA(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_RegisterEventSourceA *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_REGISTEREVENTSOURCEA];

	r = talloc(NULL, struct eventlog_RegisterEventSourceA);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_RegisterEventSourceA, r);
	}

	r->out.result = _eventlog_RegisterEventSourceA(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_RegisterEventSourceA, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_OpenBackupEventLogA(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_OpenBackupEventLogA *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_OPENBACKUPEVENTLOGA];

	r = talloc(NULL, struct eventlog_OpenBackupEventLogA);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_OpenBackupEventLogA, r);
	}

	r->out.result = _eventlog_OpenBackupEventLogA(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_OpenBackupEventLogA, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_ReadEventLogA(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_ReadEventLogA *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_READEVENTLOGA];

	r = talloc(NULL, struct eventlog_ReadEventLogA);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_ReadEventLogA, r);
	}

	r->out.result = _eventlog_ReadEventLogA(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_ReadEventLogA, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_ReportEventA(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_ReportEventA *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_REPORTEVENTA];

	r = talloc(NULL, struct eventlog_ReportEventA);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_ReportEventA, r);
	}

	r->out.result = _eventlog_ReportEventA(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_ReportEventA, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_RegisterClusterSvc(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_RegisterClusterSvc *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_REGISTERCLUSTERSVC];

	r = talloc(NULL, struct eventlog_RegisterClusterSvc);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_RegisterClusterSvc, r);
	}

	r->out.result = _eventlog_RegisterClusterSvc(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_RegisterClusterSvc, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_DeregisterClusterSvc(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_DeregisterClusterSvc *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_DEREGISTERCLUSTERSVC];

	r = talloc(NULL, struct eventlog_DeregisterClusterSvc);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_DeregisterClusterSvc, r);
	}

	r->out.result = _eventlog_DeregisterClusterSvc(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_DeregisterClusterSvc, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_WriteClusterEvents(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_WriteClusterEvents *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_WRITECLUSTEREVENTS];

	r = talloc(NULL, struct eventlog_WriteClusterEvents);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_WriteClusterEvents, r);
	}

	r->out.result = _eventlog_WriteClusterEvents(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_WriteClusterEvents, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_GetLogIntormation(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_GetLogIntormation *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_GETLOGINTORMATION];

	r = talloc(NULL, struct eventlog_GetLogIntormation);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_GetLogIntormation, r);
	}

	r->out.result = _eventlog_GetLogIntormation(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_GetLogIntormation, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}

static bool api_eventlog_FlushEventLog(pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	DATA_BLOB blob;
	struct eventlog_FlushEventLog *r;

	call = &ndr_table_eventlog.calls[NDR_EVENTLOG_FLUSHEVENTLOG];

	r = talloc(NULL, struct eventlog_FlushEventLog);
	if (r == NULL) {
		return false;
	}

	if (!prs_data_blob(&p->in_data.data, &blob, r)) {
		talloc_free(r);
		return false;
	}

	pull = ndr_pull_init_blob(&blob, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(eventlog_FlushEventLog, r);
	}

	r->out.result = _eventlog_FlushEventLog(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(eventlog_FlushEventLog, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	blob = ndr_push_blob(push);
	if (!prs_copy_data_in(&p->out_data.rdata, (const char *)blob.data, (uint32_t)blob.length)) {
		talloc_free(r);
		return false;
	}

	talloc_free(r);

	return true;
}


/* Tables */
static struct api_struct api_eventlog_cmds[] = 
{
	{"EVENTLOG_CLEAREVENTLOGW", NDR_EVENTLOG_CLEAREVENTLOGW, api_eventlog_ClearEventLogW},
	{"EVENTLOG_BACKUPEVENTLOGW", NDR_EVENTLOG_BACKUPEVENTLOGW, api_eventlog_BackupEventLogW},
	{"EVENTLOG_CLOSEEVENTLOG", NDR_EVENTLOG_CLOSEEVENTLOG, api_eventlog_CloseEventLog},
	{"EVENTLOG_DEREGISTEREVENTSOURCE", NDR_EVENTLOG_DEREGISTEREVENTSOURCE, api_eventlog_DeregisterEventSource},
	{"EVENTLOG_GETNUMRECORDS", NDR_EVENTLOG_GETNUMRECORDS, api_eventlog_GetNumRecords},
	{"EVENTLOG_GETOLDESTRECORD", NDR_EVENTLOG_GETOLDESTRECORD, api_eventlog_GetOldestRecord},
	{"EVENTLOG_CHANGENOTIFY", NDR_EVENTLOG_CHANGENOTIFY, api_eventlog_ChangeNotify},
	{"EVENTLOG_OPENEVENTLOGW", NDR_EVENTLOG_OPENEVENTLOGW, api_eventlog_OpenEventLogW},
	{"EVENTLOG_REGISTEREVENTSOURCEW", NDR_EVENTLOG_REGISTEREVENTSOURCEW, api_eventlog_RegisterEventSourceW},
	{"EVENTLOG_OPENBACKUPEVENTLOGW", NDR_EVENTLOG_OPENBACKUPEVENTLOGW, api_eventlog_OpenBackupEventLogW},
	{"EVENTLOG_READEVENTLOGW", NDR_EVENTLOG_READEVENTLOGW, api_eventlog_ReadEventLogW},
	{"EVENTLOG_REPORTEVENTW", NDR_EVENTLOG_REPORTEVENTW, api_eventlog_ReportEventW},
	{"EVENTLOG_CLEAREVENTLOGA", NDR_EVENTLOG_CLEAREVENTLOGA, api_eventlog_ClearEventLogA},
	{"EVENTLOG_BACKUPEVENTLOGA", NDR_EVENTLOG_BACKUPEVENTLOGA, api_eventlog_BackupEventLogA},
	{"EVENTLOG_OPENEVENTLOGA", NDR_EVENTLOG_OPENEVENTLOGA, api_eventlog_OpenEventLogA},
	{"EVENTLOG_REGISTEREVENTSOURCEA", NDR_EVENTLOG_REGISTEREVENTSOURCEA, api_eventlog_RegisterEventSourceA},
	{"EVENTLOG_OPENBACKUPEVENTLOGA", NDR_EVENTLOG_OPENBACKUPEVENTLOGA, api_eventlog_OpenBackupEventLogA},
	{"EVENTLOG_READEVENTLOGA", NDR_EVENTLOG_READEVENTLOGA, api_eventlog_ReadEventLogA},
	{"EVENTLOG_REPORTEVENTA", NDR_EVENTLOG_REPORTEVENTA, api_eventlog_ReportEventA},
	{"EVENTLOG_REGISTERCLUSTERSVC", NDR_EVENTLOG_REGISTERCLUSTERSVC, api_eventlog_RegisterClusterSvc},
	{"EVENTLOG_DEREGISTERCLUSTERSVC", NDR_EVENTLOG_DEREGISTERCLUSTERSVC, api_eventlog_DeregisterClusterSvc},
	{"EVENTLOG_WRITECLUSTEREVENTS", NDR_EVENTLOG_WRITECLUSTEREVENTS, api_eventlog_WriteClusterEvents},
	{"EVENTLOG_GETLOGINTORMATION", NDR_EVENTLOG_GETLOGINTORMATION, api_eventlog_GetLogIntormation},
	{"EVENTLOG_FLUSHEVENTLOG", NDR_EVENTLOG_FLUSHEVENTLOG, api_eventlog_FlushEventLog},
};

void eventlog_get_pipe_fns(struct api_struct **fns, int *n_fns)
{
	*fns = api_eventlog_cmds;
	*n_fns = sizeof(api_eventlog_cmds) / sizeof(struct api_struct);
}

NTSTATUS rpc_eventlog_init(void)
{
	return rpc_pipe_register_commands(SMB_RPC_INTERFACE_VERSION, "eventlog", "eventlog", api_eventlog_cmds, sizeof(api_eventlog_cmds) / sizeof(struct api_struct));
}
