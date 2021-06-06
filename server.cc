/* minimal COAP Server
 *
 * Copyright (C) 2018-2021 Olaf Bergmann <bergmann@tzi.org>
 */

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "common.hh"
#include "coap_pdu_internal.h"
#include "enc.h"
#include "read_file.h"
#include "rand_txt.h"

int main (void)
{
	coap_context_t * ctx = nullptr;
	coap_address_t dst;
	coap_resource_t * resource = nullptr;
	coap_endpoint_t * endpoint = nullptr;
	int result = EXIT_FAILURE;
	coap_str_const_t * ruri = coap_make_str_const("hello");
	coap_startup();

	/* resolve destination address where server should be sent */
	if (resolve_address ("localhost", "5683", &dst) < 0)
	{
		coap_log(LOG_CRIT, "failde to resolve address\n");
		goto finish;
	}

	/* create COAP context and a client session */
	ctx = coap_new_context(nullptr);

	if(!ctx || !(endpoint = coap_new_endpoint(ctx, &dst, COAP_PROTO_UDP)))
	{
		coap_log(LOG_EMERG, "cannot initialize context\n");
		goto finish;
	}

	resource = coap_resource_init (ruri, 0);

	coap_register_handler(resource, COAP_REQUEST_GET, [](auto, auto, const coap_pdu_t * request, auto, coap_pdu_t * response)
			{
			rand_txt(9);

			unsigned char msg[1024];
			int len = encrypt();

			read_txt(msg);

			coap_show_pdu(LOG_WARNING, request);
			coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
			coap_add_data(response, len, (const uint8_t *) msg);
			coap_show_pdu(LOG_WARNING, response);
			});

	coap_add_resource (ctx, resource);

	while (true) { coap_io_process (ctx, COAP_IO_WAIT); }

	result = EXIT_SUCCESS;

finish:
	coap_free_context(ctx);
	coap_cleanup();

	return result;

}
