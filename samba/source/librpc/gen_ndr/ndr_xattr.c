/* parser auto-generated by pidl */

#include "includes.h"
#include "librpc/gen_ndr/ndr_xattr.h"

_PUBLIC_ enum ndr_err_code ndr_push_tdb_xattr(struct ndr_push *ndr, int ndr_flags, const struct tdb_xattr *r)
{
	if (ndr_flags & NDR_SCALARS) {
		NDR_CHECK(ndr_push_align(ndr, 4));
		{
			uint32_t _flags_save_string = ndr->flags;
			ndr_set_flags(&ndr->flags, LIBNDR_FLAG_STR_UTF8|LIBNDR_FLAG_STR_NULLTERM);
			NDR_CHECK(ndr_push_string(ndr, NDR_SCALARS, r->name));
			ndr->flags = _flags_save_string;
		}
		NDR_CHECK(ndr_push_DATA_BLOB(ndr, NDR_SCALARS, r->value));
	}
	if (ndr_flags & NDR_BUFFERS) {
	}
	return NDR_ERR_SUCCESS;
}

_PUBLIC_ enum ndr_err_code ndr_pull_tdb_xattr(struct ndr_pull *ndr, int ndr_flags, struct tdb_xattr *r)
{
	if (ndr_flags & NDR_SCALARS) {
		NDR_CHECK(ndr_pull_align(ndr, 4));
		{
			uint32_t _flags_save_string = ndr->flags;
			ndr_set_flags(&ndr->flags, LIBNDR_FLAG_STR_UTF8|LIBNDR_FLAG_STR_NULLTERM);
			NDR_CHECK(ndr_pull_string(ndr, NDR_SCALARS, &r->name));
			ndr->flags = _flags_save_string;
		}
		NDR_CHECK(ndr_pull_DATA_BLOB(ndr, NDR_SCALARS, &r->value));
	}
	if (ndr_flags & NDR_BUFFERS) {
	}
	return NDR_ERR_SUCCESS;
}

_PUBLIC_ void ndr_print_tdb_xattr(struct ndr_print *ndr, const char *name, const struct tdb_xattr *r)
{
	ndr_print_struct(ndr, name, "tdb_xattr");
	ndr->depth++;
	ndr_print_string(ndr, "name", r->name);
	ndr_print_DATA_BLOB(ndr, "value", r->value);
	ndr->depth--;
}

_PUBLIC_ enum ndr_err_code ndr_push_tdb_xattrs(struct ndr_push *ndr, int ndr_flags, const struct tdb_xattrs *r)
{
	uint32_t cntr_xattrs_0;
	if (ndr_flags & NDR_SCALARS) {
		NDR_CHECK(ndr_push_align(ndr, 4));
		NDR_CHECK(ndr_push_uint32(ndr, NDR_SCALARS, r->num_xattrs));
		for (cntr_xattrs_0 = 0; cntr_xattrs_0 < r->num_xattrs; cntr_xattrs_0++) {
			NDR_CHECK(ndr_push_tdb_xattr(ndr, NDR_SCALARS, &r->xattrs[cntr_xattrs_0]));
		}
	}
	if (ndr_flags & NDR_BUFFERS) {
	}
	return NDR_ERR_SUCCESS;
}

_PUBLIC_ enum ndr_err_code ndr_pull_tdb_xattrs(struct ndr_pull *ndr, int ndr_flags, struct tdb_xattrs *r)
{
	uint32_t cntr_xattrs_0;
	TALLOC_CTX *_mem_save_xattrs_0;
	if (ndr_flags & NDR_SCALARS) {
		NDR_CHECK(ndr_pull_align(ndr, 4));
		NDR_CHECK(ndr_pull_uint32(ndr, NDR_SCALARS, &r->num_xattrs));
		NDR_PULL_ALLOC_N(ndr, r->xattrs, r->num_xattrs);
		_mem_save_xattrs_0 = NDR_PULL_GET_MEM_CTX(ndr);
		NDR_PULL_SET_MEM_CTX(ndr, r->xattrs, 0);
		for (cntr_xattrs_0 = 0; cntr_xattrs_0 < r->num_xattrs; cntr_xattrs_0++) {
			NDR_CHECK(ndr_pull_tdb_xattr(ndr, NDR_SCALARS, &r->xattrs[cntr_xattrs_0]));
		}
		NDR_PULL_SET_MEM_CTX(ndr, _mem_save_xattrs_0, 0);
	}
	if (ndr_flags & NDR_BUFFERS) {
	}
	return NDR_ERR_SUCCESS;
}

_PUBLIC_ void ndr_print_tdb_xattrs(struct ndr_print *ndr, const char *name, const struct tdb_xattrs *r)
{
	uint32_t cntr_xattrs_0;
	ndr_print_struct(ndr, name, "tdb_xattrs");
	ndr->depth++;
	ndr_print_uint32(ndr, "num_xattrs", r->num_xattrs);
	ndr->print(ndr, "%s: ARRAY(%d)", "xattrs", r->num_xattrs);
	ndr->depth++;
	for (cntr_xattrs_0=0;cntr_xattrs_0<r->num_xattrs;cntr_xattrs_0++) {
		char *idx_0=NULL;
		asprintf(&idx_0, "[%d]", cntr_xattrs_0);
		if (idx_0) {
			ndr_print_tdb_xattr(ndr, "xattrs", &r->xattrs[cntr_xattrs_0]);
			free(idx_0);
		}
	}
	ndr->depth--;
	ndr->depth--;
}

