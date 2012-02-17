/*
 * Unix SMB/CIFS implementation.
 * server auto-generated by pidl. DO NOT MODIFY!
 */

#include "includes.h"
#include "ntdomain.h"
#include "librpc/gen_ndr/srv_mgmt.h"

static bool api_mgmt_inq_if_ids(struct pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	struct mgmt_inq_if_ids *r;

	call = &ndr_table_mgmt.calls[NDR_MGMT_INQ_IF_IDS];

	r = talloc(talloc_tos(), struct mgmt_inq_if_ids);
	if (r == NULL) {
		return false;
	}

	pull = ndr_pull_init_blob(&p->in_data.data, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	if (p->endian) {
		pull->flags |= LIBNDR_FLAG_BIGENDIAN;
	}
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_inq_if_ids, NDR_IN, r);
	}

	ZERO_STRUCT(r->out);
	r->out.if_id_vector = talloc_zero(r, struct rpc_if_id_vector_t *);
	if (r->out.if_id_vector == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _mgmt_inq_if_ids(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_inq_if_ids, NDR_OUT | NDR_SET_VALUES, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	/*
	 * carry over the pointer count to the reply in case we are
	 * using full pointer. See NDR specification for full pointers
	 */
	push->ptr_count = pull->ptr_count;

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	p->out_data.rdata = ndr_push_blob(push);
	talloc_steal(p->mem_ctx, p->out_data.rdata.data);

	talloc_free(r);

	return true;
}

static bool api_mgmt_inq_stats(struct pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	struct mgmt_inq_stats *r;

	call = &ndr_table_mgmt.calls[NDR_MGMT_INQ_STATS];

	r = talloc(talloc_tos(), struct mgmt_inq_stats);
	if (r == NULL) {
		return false;
	}

	pull = ndr_pull_init_blob(&p->in_data.data, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	if (p->endian) {
		pull->flags |= LIBNDR_FLAG_BIGENDIAN;
	}
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_inq_stats, NDR_IN, r);
	}

	ZERO_STRUCT(r->out);
	r->out.statistics = talloc_zero(r, struct mgmt_statistics);
	if (r->out.statistics == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _mgmt_inq_stats(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_inq_stats, NDR_OUT | NDR_SET_VALUES, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	/*
	 * carry over the pointer count to the reply in case we are
	 * using full pointer. See NDR specification for full pointers
	 */
	push->ptr_count = pull->ptr_count;

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	p->out_data.rdata = ndr_push_blob(push);
	talloc_steal(p->mem_ctx, p->out_data.rdata.data);

	talloc_free(r);

	return true;
}

static bool api_mgmt_is_server_listening(struct pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	struct mgmt_is_server_listening *r;

	call = &ndr_table_mgmt.calls[NDR_MGMT_IS_SERVER_LISTENING];

	r = talloc(talloc_tos(), struct mgmt_is_server_listening);
	if (r == NULL) {
		return false;
	}

	pull = ndr_pull_init_blob(&p->in_data.data, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	if (p->endian) {
		pull->flags |= LIBNDR_FLAG_BIGENDIAN;
	}
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_is_server_listening, NDR_IN, r);
	}

	ZERO_STRUCT(r->out);
	r->out.status = talloc_zero(r, uint32_t);
	if (r->out.status == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _mgmt_is_server_listening(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_is_server_listening, NDR_OUT | NDR_SET_VALUES, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	/*
	 * carry over the pointer count to the reply in case we are
	 * using full pointer. See NDR specification for full pointers
	 */
	push->ptr_count = pull->ptr_count;

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	p->out_data.rdata = ndr_push_blob(push);
	talloc_steal(p->mem_ctx, p->out_data.rdata.data);

	talloc_free(r);

	return true;
}

static bool api_mgmt_stop_server_listening(struct pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	struct mgmt_stop_server_listening *r;

	call = &ndr_table_mgmt.calls[NDR_MGMT_STOP_SERVER_LISTENING];

	r = talloc(talloc_tos(), struct mgmt_stop_server_listening);
	if (r == NULL) {
		return false;
	}

	pull = ndr_pull_init_blob(&p->in_data.data, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	if (p->endian) {
		pull->flags |= LIBNDR_FLAG_BIGENDIAN;
	}
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_stop_server_listening, NDR_IN, r);
	}

	r->out.result = _mgmt_stop_server_listening(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_stop_server_listening, NDR_OUT | NDR_SET_VALUES, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	/*
	 * carry over the pointer count to the reply in case we are
	 * using full pointer. See NDR specification for full pointers
	 */
	push->ptr_count = pull->ptr_count;

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	p->out_data.rdata = ndr_push_blob(push);
	talloc_steal(p->mem_ctx, p->out_data.rdata.data);

	talloc_free(r);

	return true;
}

static bool api_mgmt_inq_princ_name(struct pipes_struct *p)
{
	const struct ndr_interface_call *call;
	struct ndr_pull *pull;
	struct ndr_push *push;
	enum ndr_err_code ndr_err;
	struct mgmt_inq_princ_name *r;

	call = &ndr_table_mgmt.calls[NDR_MGMT_INQ_PRINC_NAME];

	r = talloc(talloc_tos(), struct mgmt_inq_princ_name);
	if (r == NULL) {
		return false;
	}

	pull = ndr_pull_init_blob(&p->in_data.data, r);
	if (pull == NULL) {
		talloc_free(r);
		return false;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	if (p->endian) {
		pull->flags |= LIBNDR_FLAG_BIGENDIAN;
	}
	ndr_err = call->ndr_pull(pull, NDR_IN, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_inq_princ_name, NDR_IN, r);
	}

	ZERO_STRUCT(r->out);
	r->out.princ_name = talloc_zero_array(r, const char, r->in.princ_name_size);
	if (r->out.princ_name == NULL) {
		talloc_free(r);
		return false;
	}

	r->out.result = _mgmt_inq_princ_name(p, r);

	if (p->rng_fault_state) {
		talloc_free(r);
		/* Return true here, srv_pipe_hnd.c will take care */
		return true;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_FUNCTION_DEBUG(mgmt_inq_princ_name, NDR_OUT | NDR_SET_VALUES, r);
	}

	push = ndr_push_init_ctx(r);
	if (push == NULL) {
		talloc_free(r);
		return false;
	}

	/*
	 * carry over the pointer count to the reply in case we are
	 * using full pointer. See NDR specification for full pointers
	 */
	push->ptr_count = pull->ptr_count;

	ndr_err = call->ndr_push(push, NDR_OUT, r);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(r);
		return false;
	}

	p->out_data.rdata = ndr_push_blob(push);
	talloc_steal(p->mem_ctx, p->out_data.rdata.data);

	talloc_free(r);

	return true;
}


/* Tables */
static struct api_struct api_mgmt_cmds[] = 
{
	{"MGMT_INQ_IF_IDS", NDR_MGMT_INQ_IF_IDS, api_mgmt_inq_if_ids},
	{"MGMT_INQ_STATS", NDR_MGMT_INQ_STATS, api_mgmt_inq_stats},
	{"MGMT_IS_SERVER_LISTENING", NDR_MGMT_IS_SERVER_LISTENING, api_mgmt_is_server_listening},
	{"MGMT_STOP_SERVER_LISTENING", NDR_MGMT_STOP_SERVER_LISTENING, api_mgmt_stop_server_listening},
	{"MGMT_INQ_PRINC_NAME", NDR_MGMT_INQ_PRINC_NAME, api_mgmt_inq_princ_name},
};

void mgmt_get_pipe_fns(struct api_struct **fns, int *n_fns)
{
	*fns = api_mgmt_cmds;
	*n_fns = sizeof(api_mgmt_cmds) / sizeof(struct api_struct);
}

NTSTATUS rpc_mgmt_init(const struct rpc_srv_callbacks *rpc_srv_cb)
{
	return rpc_srv_register(SMB_RPC_INTERFACE_VERSION, "mgmt", "mgmt", &ndr_table_mgmt, api_mgmt_cmds, sizeof(api_mgmt_cmds) / sizeof(struct api_struct), rpc_srv_cb);
}

NTSTATUS rpc_mgmt_shutdown(void)
{
	return rpc_srv_unregister(&ndr_table_mgmt);
}