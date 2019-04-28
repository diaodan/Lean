#ifndef __MY_REDIS_H__
#define __MY_REDIS_H__
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <hiredis/hiredis.h>
#include <hiredis/read.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libev.h>


extern void displayReply(void *);




#endif //__MY_REDIS_H__

