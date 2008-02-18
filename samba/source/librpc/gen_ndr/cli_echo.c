/*
 * Unix SMB/CIFS implementation.
 * client auto-generated by pidl. DO NOT MODIFY!
 */

#include "includes.h"
#include "librpc/gen_ndr/cli_echo.h"

NTSTATUS rpccli_echo_AddOne(struct rpc_pipe_client *cli,
			    TALLOC_CTX *mem_ctx,
			    uint32_t in_data,
			    uint32_t *out_data)
{
	struct echo_AddOne r;
	NTSTATUS status;

	/* In parameters */
	r.in.in_data = in_data;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_AddOne, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_ADDONE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_AddOne, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*out_data = *r.out.out_data;

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_EchoData(struct rpc_pipe_client *cli,
			      TALLOC_CTX *mem_ctx,
			      uint32_t len,
			      uint8_t *in_data,
			      uint8_t *out_data)
{
	struct echo_EchoData r;
	NTSTATUS status;

	/* In parameters */
	r.in.len = len;
	r.in.in_data = in_data;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_EchoData, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_ECHODATA,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_EchoData, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	memcpy(out_data, r.out.out_data, r.in.len);

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_SinkData(struct rpc_pipe_client *cli,
			      TALLOC_CTX *mem_ctx,
			      uint32_t len,
			      uint8_t *data)
{
	struct echo_SinkData r;
	NTSTATUS status;

	/* In parameters */
	r.in.len = len;
	r.in.data = data;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_SinkData, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_SINKDATA,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_SinkData, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_SourceData(struct rpc_pipe_client *cli,
				TALLOC_CTX *mem_ctx,
				uint32_t len,
				uint8_t *data)
{
	struct echo_SourceData r;
	NTSTATUS status;

	/* In parameters */
	r.in.len = len;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_SourceData, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_SOURCEDATA,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_SourceData, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	memcpy(data, r.out.data, r.in.len);

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_TestCall(struct rpc_pipe_client *cli,
			      TALLOC_CTX *mem_ctx,
			      const char *s1,
			      const char **s2)
{
	struct echo_TestCall r;
	NTSTATUS status;

	/* In parameters */
	r.in.s1 = s1;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_TestCall, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_TESTCALL,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_TestCall, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*s2 = *r.out.s2;

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_TestCall2(struct rpc_pipe_client *cli,
			       TALLOC_CTX *mem_ctx,
			       uint16_t level,
			       union echo_Info *info)
{
	struct echo_TestCall2 r;
	NTSTATUS status;

	/* In parameters */
	r.in.level = level;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_TestCall2, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_TESTCALL2,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_TestCall2, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*info = *r.out.info;

	/* Return result */
	return r.out.result;
}

NTSTATUS rpccli_echo_TestSleep(struct rpc_pipe_client *cli,
			       TALLOC_CTX *mem_ctx,
			       uint32_t seconds)
{
	struct echo_TestSleep r;
	NTSTATUS status;

	/* In parameters */
	r.in.seconds = seconds;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_TestSleep, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_TESTSLEEP,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_TestSleep, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_TestEnum(struct rpc_pipe_client *cli,
			      TALLOC_CTX *mem_ctx,
			      enum echo_Enum1 *foo1,
			      struct echo_Enum2 *foo2,
			      union echo_Enum3 *foo3)
{
	struct echo_TestEnum r;
	NTSTATUS status;

	/* In parameters */
	r.in.foo1 = foo1;
	r.in.foo2 = foo2;
	r.in.foo3 = foo3;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_TestEnum, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_TESTENUM,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_TestEnum, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*foo1 = *r.out.foo1;
	*foo2 = *r.out.foo2;
	*foo3 = *r.out.foo3;

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_TestSurrounding(struct rpc_pipe_client *cli,
				     TALLOC_CTX *mem_ctx,
				     struct echo_Surrounding *data)
{
	struct echo_TestSurrounding r;
	NTSTATUS status;

	/* In parameters */
	r.in.data = data;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_TestSurrounding, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_TESTSURROUNDING,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_TestSurrounding, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	*data = *r.out.data;

	/* Return result */
	return NT_STATUS_OK;
}

NTSTATUS rpccli_echo_TestDoublePointer(struct rpc_pipe_client *cli,
				       TALLOC_CTX *mem_ctx,
				       uint16_t ***data)
{
	struct echo_TestDoublePointer r;
	NTSTATUS status;

	/* In parameters */
	r.in.data = data;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(echo_TestDoublePointer, &r);
	}

	status = cli_do_rpc_ndr(cli,
				mem_ctx,
				PI_RPCECHO,
				&ndr_table_rpcecho,
				NDR_ECHO_TESTDOUBLEPOINTER,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(echo_TestDoublePointer, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	return NT_STATUS_OK;
}

