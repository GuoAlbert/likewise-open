/*
 * Unix SMB/CIFS implementation.
 * client auto-generated by pidl. DO NOT MODIFY!
 */

#include "includes.h"
#include "librpc/gen_ndr/cli_winreg.h"

NTSTATUS rpccli_winreg_OpenHKCR(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKCR r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKCR, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKCR,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKCR, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKCU(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKCU r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKCU, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKCU,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKCU, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKLM(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKLM r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKLM, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKLM,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKLM, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKPD(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKPD r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKPD, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKPD,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKPD, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKU(struct rpc_pipe_client *cli,
			       TALLOC_CTX *mem_ctx,
			       uint16_t *system_name,
			       uint32_t access_mask,
			       struct policy_handle *handle,
			       WERROR *werror)
{
	struct winreg_OpenHKU r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKU, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKU,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKU, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_CloseKey(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_CloseKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_CloseKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_CLOSEKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_CloseKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_CreateKey(struct rpc_pipe_client *cli,
				 TALLOC_CTX *mem_ctx,
				 struct policy_handle *handle,
				 struct winreg_String name,
				 struct winreg_String keyclass,
				 uint32_t options,
				 uint32_t access_mask,
				 struct winreg_SecBuf *secdesc,
				 struct policy_handle *new_handle,
				 enum winreg_CreateAction *action_taken,
				 WERROR *werror)
{
	struct winreg_CreateKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.name = name;
	r.in.keyclass = keyclass;
	r.in.options = options;
	r.in.access_mask = access_mask;
	r.in.secdesc = secdesc;
	r.in.action_taken = action_taken;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_CreateKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_CREATEKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_CreateKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*new_handle = *r.out.new_handle;
	if (action_taken && r.out.action_taken) {
		*action_taken = *r.out.action_taken;
	}

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_DeleteKey(struct rpc_pipe_client *cli,
				 TALLOC_CTX *mem_ctx,
				 struct policy_handle *handle,
				 struct winreg_String key,
				 WERROR *werror)
{
	struct winreg_DeleteKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.key = key;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_DeleteKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_DELETEKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_DeleteKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_DeleteValue(struct rpc_pipe_client *cli,
				   TALLOC_CTX *mem_ctx,
				   struct policy_handle *handle,
				   struct winreg_String value,
				   WERROR *werror)
{
	struct winreg_DeleteValue r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.value = value;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_DeleteValue, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_DELETEVALUE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_DeleteValue, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_EnumKey(struct rpc_pipe_client *cli,
			       TALLOC_CTX *mem_ctx,
			       struct policy_handle *handle,
			       uint32_t enum_index,
			       struct winreg_StringBuf *name,
			       struct winreg_StringBuf *keyclass,
			       NTTIME *last_changed_time,
			       WERROR *werror)
{
	struct winreg_EnumKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.enum_index = enum_index;
	r.in.name = name;
	r.in.keyclass = keyclass;
	r.in.last_changed_time = last_changed_time;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_EnumKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_ENUMKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_EnumKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*name = *r.out.name;
	if (keyclass && r.out.keyclass) {
		*keyclass = *r.out.keyclass;
	}
	if (last_changed_time && r.out.last_changed_time) {
		*last_changed_time = *r.out.last_changed_time;
	}

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_EnumValue(struct rpc_pipe_client *cli,
				 TALLOC_CTX *mem_ctx,
				 struct policy_handle *handle,
				 uint32_t enum_index,
				 struct winreg_ValNameBuf *name,
				 enum winreg_Type *type,
				 uint8_t *value,
				 uint32_t *size,
				 uint32_t *length,
				 WERROR *werror)
{
	struct winreg_EnumValue r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.enum_index = enum_index;
	r.in.name = name;
	r.in.type = type;
	r.in.value = value;
	r.in.size = size;
	r.in.length = length;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_EnumValue, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_ENUMVALUE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_EnumValue, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*name = *r.out.name;
	if (type && r.out.type) {
		*type = *r.out.type;
	}
	if (value && r.out.value) {
		memcpy(value, r.out.value, *r.in.size);
	}
	if (size && r.out.size) {
		*size = *r.out.size;
	}
	if (length && r.out.length) {
		*length = *r.out.length;
	}

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_FlushKey(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_FlushKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_FlushKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_FLUSHKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_FlushKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_GetKeySecurity(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      struct policy_handle *handle,
				      uint32_t sec_info,
				      struct KeySecurityData *sd,
				      WERROR *werror)
{
	struct winreg_GetKeySecurity r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.sec_info = sec_info;
	r.in.sd = sd;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_GetKeySecurity, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_GETKEYSECURITY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_GetKeySecurity, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*sd = *r.out.sd;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_LoadKey(struct rpc_pipe_client *cli,
			       TALLOC_CTX *mem_ctx,
			       struct policy_handle *handle,
			       struct winreg_String *keyname,
			       struct winreg_String *filename,
			       WERROR *werror)
{
	struct winreg_LoadKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.keyname = keyname;
	r.in.filename = filename;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_LoadKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_LOADKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_LoadKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_NotifyChangeKeyValue(struct rpc_pipe_client *cli,
					    TALLOC_CTX *mem_ctx,
					    struct policy_handle *handle,
					    uint8_t watch_subtree,
					    uint32_t notify_filter,
					    uint32_t unknown,
					    struct winreg_String string1,
					    struct winreg_String string2,
					    uint32_t unknown2,
					    WERROR *werror)
{
	struct winreg_NotifyChangeKeyValue r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.watch_subtree = watch_subtree;
	r.in.notify_filter = notify_filter;
	r.in.unknown = unknown;
	r.in.string1 = string1;
	r.in.string2 = string2;
	r.in.unknown2 = unknown2;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_NotifyChangeKeyValue, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_NOTIFYCHANGEKEYVALUE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_NotifyChangeKeyValue, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenKey(struct rpc_pipe_client *cli,
			       TALLOC_CTX *mem_ctx,
			       struct policy_handle *parent_handle,
			       struct winreg_String keyname,
			       uint32_t unknown,
			       uint32_t access_mask,
			       struct policy_handle *handle,
			       WERROR *werror)
{
	struct winreg_OpenKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.parent_handle = parent_handle;
	r.in.keyname = keyname;
	r.in.unknown = unknown;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_QueryInfoKey(struct rpc_pipe_client *cli,
				    TALLOC_CTX *mem_ctx,
				    struct policy_handle *handle,
				    struct winreg_String *classname,
				    uint32_t *num_subkeys,
				    uint32_t *max_subkeylen,
				    uint32_t *max_classlen,
				    uint32_t *num_values,
				    uint32_t *max_valnamelen,
				    uint32_t *max_valbufsize,
				    uint32_t *secdescsize,
				    NTTIME *last_changed_time,
				    WERROR *werror)
{
	struct winreg_QueryInfoKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.classname = classname;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_QueryInfoKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_QUERYINFOKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_QueryInfoKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*classname = *r.out.classname;
	*num_subkeys = *r.out.num_subkeys;
	*max_subkeylen = *r.out.max_subkeylen;
	*max_classlen = *r.out.max_classlen;
	*num_values = *r.out.num_values;
	*max_valnamelen = *r.out.max_valnamelen;
	*max_valbufsize = *r.out.max_valbufsize;
	*secdescsize = *r.out.secdescsize;
	*last_changed_time = *r.out.last_changed_time;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_QueryValue(struct rpc_pipe_client *cli,
				  TALLOC_CTX *mem_ctx,
				  struct policy_handle *handle,
				  struct winreg_String value_name,
				  enum winreg_Type *type,
				  uint8_t *data,
				  uint32_t *data_size,
				  uint32_t *value_length,
				  WERROR *werror)
{
	struct winreg_QueryValue r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.value_name = value_name;
	r.in.type = type;
	r.in.data = data;
	r.in.data_size = data_size;
	r.in.value_length = value_length;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_QueryValue, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_QUERYVALUE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_QueryValue, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	if (type && r.out.type) {
		*type = *r.out.type;
	}
	if (data && r.out.data) {
		memcpy(data, r.out.data, *r.in.data_size);
	}
	if (data_size && r.out.data_size) {
		*data_size = *r.out.data_size;
	}
	if (value_length && r.out.value_length) {
		*value_length = *r.out.value_length;
	}

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_ReplaceKey(struct rpc_pipe_client *cli,
				  TALLOC_CTX *mem_ctx,
				  WERROR *werror)
{
	struct winreg_ReplaceKey r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_ReplaceKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_REPLACEKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_ReplaceKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_RestoreKey(struct rpc_pipe_client *cli,
				  TALLOC_CTX *mem_ctx,
				  struct policy_handle *handle,
				  struct winreg_String *filename,
				  uint32_t flags,
				  WERROR *werror)
{
	struct winreg_RestoreKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.filename = filename;
	r.in.flags = flags;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_RestoreKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_RESTOREKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_RestoreKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_SaveKey(struct rpc_pipe_client *cli,
			       TALLOC_CTX *mem_ctx,
			       struct policy_handle *handle,
			       struct winreg_String *filename,
			       struct KeySecurityAttribute *sec_attrib,
			       WERROR *werror)
{
	struct winreg_SaveKey r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.filename = filename;
	r.in.sec_attrib = sec_attrib;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_SaveKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_SAVEKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_SaveKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_SetKeySecurity(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      struct policy_handle *handle,
				      uint32_t access_mask,
				      struct KeySecurityData *sd,
				      WERROR *werror)
{
	struct winreg_SetKeySecurity r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.access_mask = access_mask;
	r.in.sd = sd;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_SetKeySecurity, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_SETKEYSECURITY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_SetKeySecurity, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_SetValue(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				struct policy_handle *handle,
				struct winreg_String name,
				enum winreg_Type type,
				uint8_t *data,
				uint32_t size,
				WERROR *werror)
{
	struct winreg_SetValue r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;
	r.in.name = name;
	r.in.type = type;
	r.in.data = data;
	r.in.size = size;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_SetValue, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_SETVALUE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_SetValue, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_UnLoadKey(struct rpc_pipe_client *cli,
				 TALLOC_CTX *mem_ctx,
				 WERROR *werror)
{
	struct winreg_UnLoadKey r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_UnLoadKey, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_UNLOADKEY,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_UnLoadKey, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_InitiateSystemShutdown(struct rpc_pipe_client *cli,
					      TALLOC_CTX *mem_ctx,
					      uint16_t *hostname,
					      struct initshutdown_String *message,
					      uint32_t timeout,
					      uint8_t force_apps,
					      uint8_t reboot,
					      WERROR *werror)
{
	struct winreg_InitiateSystemShutdown r;
	NTSTATUS status;

	/* In parameters */
	r.in.hostname = hostname;
	r.in.message = message;
	r.in.timeout = timeout;
	r.in.force_apps = force_apps;
	r.in.reboot = reboot;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_InitiateSystemShutdown, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_INITIATESYSTEMSHUTDOWN,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_InitiateSystemShutdown, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_AbortSystemShutdown(struct rpc_pipe_client *cli,
					   TALLOC_CTX *mem_ctx,
					   uint16_t *server,
					   WERROR *werror)
{
	struct winreg_AbortSystemShutdown r;
	NTSTATUS status;

	/* In parameters */
	r.in.server = server;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_AbortSystemShutdown, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_ABORTSYSTEMSHUTDOWN,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_AbortSystemShutdown, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_GetVersion(struct rpc_pipe_client *cli,
				  TALLOC_CTX *mem_ctx,
				  struct policy_handle *handle,
				  uint32_t *version,
				  WERROR *werror)
{
	struct winreg_GetVersion r;
	NTSTATUS status;

	/* In parameters */
	r.in.handle = handle;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_GetVersion, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_GETVERSION,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_GetVersion, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*version = *r.out.version;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKCC(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKCC r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKCC, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKCC,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKCC, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKDD(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKDD r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKDD, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKDD,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKDD, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_QueryMultipleValues(struct rpc_pipe_client *cli,
					   TALLOC_CTX *mem_ctx,
					   struct policy_handle *key_handle,
					   struct QueryMultipleValue *values,
					   uint32_t num_values,
					   uint8_t *buffer,
					   uint32_t *buffer_size,
					   WERROR *werror)
{
	struct winreg_QueryMultipleValues r;
	NTSTATUS status;

	/* In parameters */
	r.in.key_handle = key_handle;
	r.in.values = values;
	r.in.num_values = num_values;
	r.in.buffer = buffer;
	r.in.buffer_size = buffer_size;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_QueryMultipleValues, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_QUERYMULTIPLEVALUES,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_QueryMultipleValues, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	memcpy(values, r.out.values, r.in.num_values);
	if (buffer && r.out.buffer) {
		memcpy(buffer, r.out.buffer, *r.in.buffer_size);
	}
	*buffer_size = *r.out.buffer_size;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_InitiateSystemShutdownEx(struct rpc_pipe_client *cli,
						TALLOC_CTX *mem_ctx,
						uint16_t *hostname,
						struct initshutdown_String *message,
						uint32_t timeout,
						uint8_t force_apps,
						uint8_t reboot,
						uint32_t reason,
						WERROR *werror)
{
	struct winreg_InitiateSystemShutdownEx r;
	NTSTATUS status;

	/* In parameters */
	r.in.hostname = hostname;
	r.in.message = message;
	r.in.timeout = timeout;
	r.in.force_apps = force_apps;
	r.in.reboot = reboot;
	r.in.reason = reason;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_InitiateSystemShutdownEx, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_INITIATESYSTEMSHUTDOWNEX,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_InitiateSystemShutdownEx, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_SaveKeyEx(struct rpc_pipe_client *cli,
				 TALLOC_CTX *mem_ctx,
				 WERROR *werror)
{
	struct winreg_SaveKeyEx r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_SaveKeyEx, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_SAVEKEYEX,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_SaveKeyEx, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKPT(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKPT r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKPT, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKPT,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKPT, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_OpenHKPN(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint16_t *system_name,
				uint32_t access_mask,
				struct policy_handle *handle,
				WERROR *werror)
{
	struct winreg_OpenHKPN r;
	NTSTATUS status;

	/* In parameters */
	r.in.system_name = system_name;
	r.in.access_mask = access_mask;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_OpenHKPN, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_OPENHKPN,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_OpenHKPN, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*handle = *r.out.handle;

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_winreg_QueryMultipleValues2(struct rpc_pipe_client *cli,
					    TALLOC_CTX *mem_ctx,
					    WERROR *werror)
{
	struct winreg_QueryMultipleValues2 r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(winreg_QueryMultipleValues2, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_WINREG,
				&ndr_table_winreg,
				NDR_WINREG_QUERYMULTIPLEVALUES2,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(winreg_QueryMultipleValues2, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

