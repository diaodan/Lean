#ifndef __MY_NET_H__
#define __MY_NET_H__

#include "myhead.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

int ll_redis_connect(const char *host_ip, const char *host_port);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__MY_NET_H__
