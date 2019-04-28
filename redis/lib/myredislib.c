#include "myredis.h"

void displayReply(void *rp)
{
    redisReply *reply = (struct redisReply*)rp;
    char buff[128];
    size_t i;

    switch(reply->type) {
        case REDIS_REPLY_STATUS:
            write(STDOUT_FILENO, reply->str, reply->len);
            write(STDOUT_FILENO, "\n", strlen("\n"));
            break;
        case REDIS_REPLY_ERROR:
            write(STDOUT_FILENO, reply->str, reply->len);
            write(STDOUT_FILENO, "\n", strlen("\n"));
            break;
        case REDIS_REPLY_INTEGER:
            snprintf(buff, 127, "%ld\n", reply->integer); 
            write(STDOUT_FILENO, buff, strlen(buff));
            break;
        case REDIS_REPLY_NIL:
            write(STDOUT_FILENO, "nil\n", strlen("nil\n"));
            break;
        case REDIS_REPLY_STRING:
            write(STDOUT_FILENO, reply->str, reply->len);
            write(STDOUT_FILENO, "\n", strlen("\n"));
            break;
        case REDIS_REPLY_ARRAY:
            for (i = 0; i < reply->elements; i++) {
                displayReply(reply->element[i]);
            }
            break;
        default:
            write(STDOUT_FILENO, "unknow reply type\n",
                        strlen("unknow reply type\n"));
            break;
    }
    return ;
}

void testPipe(redisContext *pContext)
{
    redisReply *reply = NULL;
    
    redisAppendCommand(pContext, "set lilu good");
    redisAppendCommand(pContext, "get lilu");
    redisGetReply(pContext, (void*)&reply);
    displayReply(reply);
    freeReplyObject(reply);
    reply = NULL;
    redisGetReply(pContext, (void*)&reply);
    displayReply(reply);
    freeReplyObject(reply);
    return ;
}

void getSubscriber(redisContext *pContext)
{
    void *reply = NULL;

    reply = redisCommand(pContext, "subscribe lilu");
    displayReply(reply);
    freeReplyObject(reply);
    while(redisGetReply(pContext, &reply) == REDIS_OK) {
        displayReply(reply);
        freeReplyObject(reply);
    }
    return ;
}

