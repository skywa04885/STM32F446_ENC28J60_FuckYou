#include "bootp.h"

static u8 dhcp_cookie[4] = { 99, 130, 83, 99 };

bootp_oparam_t *bootp_init_dhcp_options(bootp_pkt_t *bootp_pkt)
{
	bootp_oparam_dhcp_cookie_t *cookie = (bootp_oparam_dhcp_cookie_t *) bootp_pkt->body.payload;
	memcpy(cookie->values, dhcp_cookie, 4);
	return (bootp_oparam_t *) cookie->next;
}

bootp_oparam_t *bootp_oparam_add_u8(u8 code, u8 val, bootp_oparam_t *param)
{
	param->code = code;
	param->len = sizeof (bootp_oparam_u8_t);
	((bootp_oparam_u8_t *) param->body)->val = val;
	return (bootp_oparam_t *) ((bootp_oparam_u8_t *) param->body)->next;
}

bootp_oparam_t *bootp_oparam_add_addr(u8 code, u8 *addr, bootp_oparam_t *param)
{
	param->code = code;
	param->len = sizeof (bootp_oparam_addr_t);
	memcpy(((bootp_oparam_addr_t *) param->body)->addr, addr, 4);
	return (bootp_oparam_t *) ((bootp_oparam_addr_t *) param->body)->next;
}

bootp_oparam_t *bootp_oparam_end(bootp_oparam_t *param)
{
	param->code = BOOTP_OPTION_CODE_END;
	return ++param;
}

bootp_oparam_t *bootp_oparam_parser_init_dhcp(bootp_pkt_t *bootp_pkt)
{
	return (bootp_oparam_t *) &bootp_pkt->body.payload[4];
}

bootp_oparam_t *bootp_oparam_parser_next(bootp_oparam_t *param)
{
	param = (bootp_oparam_t *) &param->body[param->len];

	while (param->code == BOOTP_OPTION_CODE_PAD) ++param;

	if (param->code == BOOTP_OPTION_CODE_END) return NULL;
	return param;
}
