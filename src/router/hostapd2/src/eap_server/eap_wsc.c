/*
 * EAP-WSC server for Wi-Fi Protected Setup
 * Copyright (c) 2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "eap_i.h"
#include "eap_common/eap_wsc_common.h"


struct wps_data {
	int registrar;
	int msg_num;
};


static struct wps_data * wps_init(int registrar)
{
	struct wps_data *data = os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;
	data->registrar = registrar;
	data->msg_num = 1;
	return data;
}


static void wps_deinit(struct wps_data *data)
{
	os_free(data);
}


enum wps_process_res {
	WPS_DONE, WPS_CONTINUE, WPS_FAILURE, WPS_PENDING
};

static enum wps_process_res wps_process_msg(struct wps_data *wps, u8 op_code,
					    const u8 *msg, size_t msg_len)
{
	/* TODO: proper processing and/or sending to an external process */

	wpa_hexdump(MSG_MSGDUMP, "WPS: Received message", msg, msg_len);
	if ((wps->registrar && (wps->msg_num & 1) == 0) ||
	    (!wps->registrar && (wps->msg_num & 1) == 1)) {
		wpa_printf(MSG_DEBUG, "WPS: Unexpected message number %d",
			   wps->msg_num);
		return WPS_FAILURE;
	}

	if (wps->msg_num <= 8 && op_code == WSC_MSG) {
		wpa_printf(MSG_DEBUG, "WPS: Process M%d", wps->msg_num);
	} else if (wps->registrar && wps->msg_num == 3 &&
		   op_code == WSC_ACK) {
		wpa_printf(MSG_DEBUG, "WPS: Process ACK to M2/M2D");
		/* could send out next M2/M2D */
		return WPS_DONE;
	} else if (wps->registrar && wps->msg_num == 3 &&
		   op_code == WSC_Done) {
		wpa_printf(MSG_DEBUG, "WPS: Process Done to M2/M2D");
		return WPS_DONE;
	} else if (wps->msg_num <= 8 && op_code == WSC_Done) {
		wpa_printf(MSG_DEBUG, "WPS: Process Done prior to completion");
		return WPS_DONE;
	} else if (wps->msg_num <= 8 && op_code == WSC_ACK) {
		wpa_printf(MSG_DEBUG, "WPS: Process ACK prior to completion");
		return WPS_DONE;
	} else if (wps->msg_num <= 8 && op_code == WSC_NACK) {
		wpa_printf(MSG_DEBUG, "WPS: Process NACK prior to completion");
		return WPS_DONE;
	} else if (wps->registrar && wps->msg_num == 9 &&
		   op_code == WSC_Done) {
		wpa_printf(MSG_DEBUG, "WPS: Process Done");
		return WPS_DONE;
	} else if (!wps->registrar && wps->msg_num == 10 &&
		   op_code == WSC_ACK) {
		wpa_printf(MSG_DEBUG, "WPS: Process ACK");
		return WPS_DONE;
	} else {
		wpa_printf(MSG_DEBUG, "WPS: Unexpected Op-Code %d "
			   "(msg_num=%d)", op_code, wps->msg_num);
		return WPS_FAILURE;
	}

	wps->msg_num++;
	return WPS_CONTINUE;
}


static u8 * wps_get_msg(struct wps_data *wps, size_t *msg_len, u8 *op_code)
{
	u8 *msg;

	/* TODO: proper processing and/or query from an external process */

	if ((wps->registrar && (wps->msg_num & 1) == 1) ||
	    (!wps->registrar && (wps->msg_num & 1) == 0)) {
		wpa_printf(MSG_DEBUG, "WPS: Unexpected request for message "
			   "number %d", wps->msg_num);
		return NULL;
	}

	if (wps->msg_num == 7 || wps->msg_num == 8) {
		msg = os_zalloc(2000);
		if (msg == NULL)
			return NULL;
		*msg_len = 2000;
		*op_code = WSC_MSG;
		*msg = WSC_MSG;
		wpa_printf(MSG_DEBUG, "WPS: Send M%d", wps->msg_num);
	} else if (wps->msg_num <= 6) {
		msg = os_zalloc(1);
		if (msg == NULL)
			return NULL;
		*msg_len = 1;
		*op_code = WSC_MSG;
		*msg = WSC_MSG;
		wpa_printf(MSG_DEBUG, "WPS: Send M%d", wps->msg_num);
	} else if (!wps->registrar && wps->msg_num == 9) {
		msg = os_zalloc(1);
		if (msg == NULL)
			return NULL;
		*msg_len = 1;
		*op_code = WSC_Done;
		*msg = WSC_Done;
		wpa_printf(MSG_DEBUG, "WPS: Send Done");
	} else
		return NULL;

	wps->msg_num++;
	return msg;
}


struct eap_wsc_data {
	enum { START, MSG, FRAG_ACK, WAIT_FRAG_ACK, DONE, FAIL } state;
	int registrar;
	u8 *in_buf, *out_buf, in_op_code, out_op_code;
	size_t in_len, in_used, out_len, out_used;
	size_t fragment_size;
	struct wps_data *wps;
};


static const char * eap_wsc_state_txt(int state)
{
	switch (state) {
	case START:
		return "START";
	case MSG:
		return "MSG";
	case FRAG_ACK:
		return "FRAG_ACK";
	case WAIT_FRAG_ACK:
		return "WAIT_FRAG_ACK";
	case DONE:
		return "DONE";
	case FAIL:
		return "FAIL";
	default:
		return "?";
	}
}


static void eap_wsc_state(struct eap_wsc_data *data, int state)
{
	wpa_printf(MSG_DEBUG, "EAP-WSC: %s -> %s",
		   eap_wsc_state_txt(data->state),
		   eap_wsc_state_txt(state));
	data->state = state;
}


static void * eap_wsc_init(struct eap_sm *sm)
{
	struct eap_wsc_data *data;
	int registrar;

	if (sm->identity && sm->identity_len == WSC_ID_REGISTRAR_LEN &&
	    os_memcmp(sm->identity, WSC_ID_REGISTRAR, WSC_ID_REGISTRAR_LEN) ==
	    0)
		registrar = 0; /* Supplicant is Registrar */
	else if (sm->identity && sm->identity_len == WSC_ID_ENROLLEE_LEN &&
		 os_memcmp(sm->identity, WSC_ID_ENROLLEE, WSC_ID_ENROLLEE_LEN)
		 == 0)
		registrar = 1; /* Supplicant is Enrollee */
	else {
		wpa_hexdump_ascii(MSG_INFO, "EAP-WSC: Unexpected identity",
				  sm->identity, sm->identity_len);
		return NULL;
	}

	data = os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;
	data->state = registrar ? START : MSG;
	data->registrar = registrar;
	data->wps = wps_init(registrar);
	if (data->wps == NULL) {
		os_free(data);
		return NULL;
	}
	data->fragment_size = WSC_FRAGMENT_SIZE;

	return data;
}


static void eap_wsc_reset(struct eap_sm *sm, void *priv)
{
	struct eap_wsc_data *data = priv;
	os_free(data->in_buf);
	os_free(data->out_buf);
	wps_deinit(data->wps);
	os_free(data);
}


static struct wpabuf * eap_wsc_build_start(struct eap_sm *sm,
					   struct eap_wsc_data *data, u8 id)
{
	struct wpabuf *req;

	req = eap_msg_alloc(EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC, 2,
			    EAP_CODE_REQUEST, id);
	if (req == NULL) {
		wpa_printf(MSG_ERROR, "EAP-WSC: Failed to allocate memory for "
			   "request");
		return NULL;
	}

	wpa_printf(MSG_DEBUG, "EAP-WSC: Send WSC/Start");
	wpabuf_put_u8(req, WSC_Start); /* Op-Code */
	wpabuf_put_u8(req, 0); /* Flags */

	return req;
}


static struct wpabuf * eap_wsc_build_msg(struct eap_sm *sm,
					 struct eap_wsc_data *data, u8 id)
{
	struct wpabuf *req;
	u8 flags;
	size_t send_len, plen;

	if (data->out_buf == NULL) {
		data->out_buf = wps_get_msg(data->wps, &data->out_len,
					    &data->out_op_code);
		if (data->out_buf == NULL) {
			wpa_printf(MSG_DEBUG, "EAP-WSC: Failed to receive "
				   "message from WPS");
			return NULL;
		}
	}

	flags = 0;
	send_len = data->out_len - data->out_used;
	if (2 + send_len > data->fragment_size) {
		send_len = data->fragment_size - 2;
		flags |= WSC_FLAGS_MF;
		if (data->out_used == 0) {
			flags |= WSC_FLAGS_LF;
			send_len -= 2;
		}
	}
	plen = 2 + send_len;
	if (flags & WSC_FLAGS_LF)
		plen += 2;
	req = eap_msg_alloc(EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC, plen,
			    EAP_CODE_REQUEST, id);
	if (req == NULL) {
		wpa_printf(MSG_ERROR, "EAP-WSC: Failed to allocate memory for "
			   "request");
		return NULL;
	}

	wpabuf_put_u8(req, data->out_op_code); /* Op-Code */
	wpabuf_put_u8(req, flags); /* Flags */
	if (flags & WSC_FLAGS_LF)
		wpabuf_put_be16(req, data->out_len);

	wpabuf_put_data(req, data->out_buf + data->out_used, send_len);
	data->out_used += send_len;

	if (data->out_used == data->out_len) {
		wpa_printf(MSG_DEBUG, "EAP-WSC: Sending out %u bytes "
			   "(message sent completely)", send_len);
		os_free(data->out_buf);
		data->out_buf = NULL;
		data->out_len = data->out_used = 0;
		eap_wsc_state(data, MSG);
	} else {
		wpa_printf(MSG_DEBUG, "EAP-WSC: Sending out %u bytes "
			   "(%u more to send)", send_len,
			   data->out_len - data->out_used);
		eap_wsc_state(data, WAIT_FRAG_ACK);
	}

	return req;
}


static struct wpabuf * eap_wsc_buildReq(struct eap_sm *sm, void *priv, u8 id)
{
	struct eap_wsc_data *data = priv;

	switch (data->state) {
	case START:
		return eap_wsc_build_start(sm, data, id);
	case FRAG_ACK:
		return eap_wsc_build_frag_ack(id, EAP_CODE_REQUEST);
	case MSG:
		return eap_wsc_build_msg(sm, data, id);
	default:
		wpa_printf(MSG_DEBUG, "EAP-WSC: Unexpected state %d in "
			   "buildReq", data->state);
		return NULL;
	}
}


static Boolean eap_wsc_check(struct eap_sm *sm, void *priv,
			     struct wpabuf *respData)
{
	const u8 *pos;
	size_t len;

	pos = eap_hdr_validate(EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC,
			       respData, &len);
	if (pos == NULL || len < 2) {
		wpa_printf(MSG_INFO, "EAP-WSC: Invalid frame");
		return TRUE;
	}

	return FALSE;
}


static void eap_wsc_process(struct eap_sm *sm, void *priv,
			    struct wpabuf *respData)
{
	struct eap_wsc_data *data = priv;
	const u8 *start, *pos, *end;
	size_t len;
	u8 op_code, flags;
	u16 message_length = 0;
	const u8 *msg;
	size_t msg_len;
	enum wps_process_res res;

	pos = eap_hdr_validate(EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC,
			       respData, &len);
	if (pos == NULL || len < 2)
		return; /* Should not happen; message already verified */

	start = pos;
	end = start + len;

	op_code = *pos++;
	flags = *pos++;
	if (flags & WSC_FLAGS_LF) {
		if (end - pos < 2) {
			wpa_printf(MSG_DEBUG, "EAP-WSC: Message underflow");
			return;
		}
		message_length = WPA_GET_BE16(pos);
		pos += 2;

		if (message_length < end - pos) {
			wpa_printf(MSG_DEBUG, "EAP-WSC: Invalid Message "
				   "Length");
			return;
		}
	}

	wpa_printf(MSG_DEBUG, "EAP-WSC: Received packet: Op-Code %d "
		   "Flags 0x%x Message Length %d",
		   op_code, flags, message_length);

	if (data->state == WAIT_FRAG_ACK) {
		if (op_code != WSC_FRAG_ACK) {
			wpa_printf(MSG_DEBUG, "EAP-WSC: Unexpected Op-Code %d "
				   "in WAIT_FRAG_ACK state", op_code);
			eap_wsc_state(data, FAIL);
			return;
		}
		wpa_printf(MSG_DEBUG, "EAP-WSC: Fragment acknowledged");
		eap_wsc_state(data, MSG);
		return;
	}

	if (op_code != WSC_ACK && op_code != WSC_NACK && op_code != WSC_MSG &&
	    op_code != WSC_Done) {
		wpa_printf(MSG_DEBUG, "EAP-WSC: Unexpected Op-Code %d",
			   op_code);
		eap_wsc_state(data, FAIL);
		return;
	}

	if (data->in_buf) {
		/* Process continuation of a pending message */
		if (op_code != data->in_op_code) {
			wpa_printf(MSG_DEBUG, "EAP-WSC: Unexpected Op-Code %d "
				   "in fragment (expected %d)",
				   op_code, data->in_op_code);
			eap_wsc_state(data, FAIL);
			return;
		}

		if (data->in_used + (end - pos) > data->in_len) {
			wpa_printf(MSG_DEBUG, "EAP-WSC: Fragment overflow");
			eap_wsc_state(data, FAIL);
			return;
		}

		os_memcpy(data->in_buf + data->in_used, pos, end - pos);
		data->in_used += end - pos;
		wpa_printf(MSG_DEBUG, "EAP-WSC: Received %u bytes, waiting "
			   "for %u bytes more", end - pos,
			   data->in_len - data->in_used);
	}

	if (flags & WSC_FLAGS_MF) {
		if (data->in_buf == NULL && !(flags & WSC_FLAGS_LF)) {
			wpa_printf(MSG_DEBUG, "EAP-WSC: No Message Length "
				   "field in a fragmented packet");
			return;
		}

		if (data->in_buf == NULL) {
			/* First fragment of the message */
			data->in_buf = os_malloc(message_length);
			if (data->in_buf == NULL) {
				wpa_printf(MSG_DEBUG, "EAP-WSC: No memory for "
					   "message");
				return;
			}
			data->in_len = message_length;
			data->in_used = end - pos;
			data->in_op_code = op_code;
			os_memcpy(data->in_buf, pos, data->in_used);
			wpa_printf(MSG_DEBUG, "EAP-WSC: Received %u bytes in "
				   "first fragment, waiting for %u bytes more",
				   data->in_used,
				   data->in_len - data->in_used);
		}

		eap_wsc_state(data, FRAG_ACK);
		return;
	}

	if (data->in_buf) {
		msg = data->in_buf;
		msg_len = data->in_len;
	} else {
		msg = pos;
		msg_len = end - pos;
	}

	res = wps_process_msg(data->wps, op_code, msg, msg_len);
	switch (res) {
	case WPS_DONE:
		wpa_printf(MSG_DEBUG, "EAP-WSC: WPS processing completed "
			   "successfully - report EAP failure");
		eap_wsc_state(data, FAIL);
		break;
	case WPS_CONTINUE:
		eap_wsc_state(data, MSG);
		break;
	case WPS_FAILURE:
		wpa_printf(MSG_DEBUG, "EAP-WSC: WPS processing failed");
		eap_wsc_state(data, FAIL);
		break;
	case WPS_PENDING:
		wpa_printf(MSG_DEBUG, "EAP-WSC: WPS processing pending");
		sm->method_pending = METHOD_PENDING_WAIT;
		break;
	}

	os_free(data->in_buf);
	data->in_buf = NULL;
	data->in_len = data->in_used = 0;
}


static Boolean eap_wsc_isDone(struct eap_sm *sm, void *priv)
{
	struct eap_wsc_data *data = priv;
	return data->state == FAIL;
}


static Boolean eap_wsc_isSuccess(struct eap_sm *sm, void *priv)
{
	/* EAP-WSC will always result in EAP-Failure */
	return FALSE;
}


int eap_server_wsc_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_server_method_alloc(EAP_SERVER_METHOD_INTERFACE_VERSION,
				      EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC,
				      "WSC");
	if (eap == NULL)
		return -1;

	eap->init = eap_wsc_init;
	eap->reset = eap_wsc_reset;
	eap->buildReq = eap_wsc_buildReq;
	eap->check = eap_wsc_check;
	eap->process = eap_wsc_process;
	eap->isDone = eap_wsc_isDone;
	eap->isSuccess = eap_wsc_isSuccess;

	ret = eap_server_method_register(eap);
	if (ret)
		eap_server_method_free(eap);
	return ret;
}
