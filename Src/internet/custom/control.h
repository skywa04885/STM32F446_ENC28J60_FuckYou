#ifndef _SRC_INTERNET_CUSTOM_CONTROL_H
#define _SRC_INTERNET_CUSTOM_CONTROL_H

typedef enum
{
	CONTROL_PKT_TYPE_CONNECT_REQUEST = 0,
	CONTROL_PKT_TYPE_CONNECT_REPLY,
	CONTROL_PKT_TYPE_INFO
} control_pkt_type_t;

typedef struct __attribute__ (( packed ))
{

} control_pkt_t;

#endif
