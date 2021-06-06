/* minimal COAP CLient
 *
 * Copyright (C) 2018-2021 Olaf Bergmann <bergmann@tzi.org>
 */

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "common.hh"
#include "coap_pdu_internal.h"
#include "dec.h"

int main (void)
{
	coap_context_t * ctx = nullptr;
	coap_session_t * session = nullptr;
	coap_address_t dst;
	coap_pdu_t * pdu = nullptr;
	int result = EXIT_FAILURE;

	coap_startup();

	/* resolve destination address where server should be sent */
	if (resolve_address ("localhost", "5683", &dst) < 0)
	{
		coap_log(LOG_CRIT, "failed to resolve address\n");
		goto finish;
	}

	/* create CoAP context and a client session */
	ctx = coap_new_context (nullptr);

	if(!ctx || !(session = coap_new_client_session(ctx, nullptr, &dst, COAP_PROTO_UDP)))
	{
		coap_log(LOG_EMERG, "cannot create client session\n");
		goto finish;
	}

	/* coap_register_response_handler(ctx, response_handler); */
	coap_register_response_handler(ctx, [](auto, auto, const coap_pdu_t * received, auto)
			{
			coap_show_pdu(LOG_WARNING, received);

			int len = received->used_size - (received->data - received->token);

			unsigned char * msg;

			msg = new unsigned char [len];

			//for(int i = 0; i < len; i++)
			//	msg[i] = (char)received->data[i];

			msg = reinterpret_cast<unsigned char *>(received->data);

			//for(int i = 0; i < len; i++)
			//	std::cout << i << "-" << msg[i] << " ";
			//std::cout << std::endl;
			decrypt(msg, len);

			return COAP_RESPONSE_OK;
			});

	/* construct CoAP message */
	pdu = coap_pdu_init(COAP_MESSAGE_CON,
			COAP_REQUEST_CODE_GET,
			coap_new_message_id(session),
			coap_session_max_pdu_size(session));

	if (!pdu)
	{
		coap_log(LOG_EMERG, "cannot create PDU\n");
		goto finish;
	}

	/* add a Uri-path option */
	coap_add_option(pdu, COAP_OPTION_URI_PATH, 5,
			reinterpret_cast<const uint8_t *> ("hello"));

	coap_show_pdu(LOG_WARNING, pdu);
	/* and send the PDU */
	coap_send(session, pdu);

	coap_io_process (ctx, COAP_IO_WAIT);

	result = EXIT_SUCCESS;
	
finish:
	coap_session_release(session);
	coap_free_context(ctx);
	coap_cleanup();

	return result;
}
