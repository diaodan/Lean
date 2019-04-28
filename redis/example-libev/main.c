#include "myredis.h"

void disconnect_callback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        printf("disconnect error %s\n", c->errstr);
        return ;
    }
    printf("disconnecting ...\n");
}

void connect_callback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        printf("connect error: %s\n", c->errstr);
        return ;
    }
    printf("connecting ...\n");
}

void *test_libev_pthread(void *arg)
{
    redisAsyncContext *c = (redisAsyncContext *)arg;
    int ret = 0;
    
    redisLibevAttach(EV_DEFAULT_ c);
    ret = redisAsyncSetConnectCallback(c, connect_callback);
    if (ret == REDIS_ERR) {
        printf("set callbackfailed\n");
        return -1;
    }
    ret = redisAsyncSetDisconnectCallback(c, disconnect_callback);
    if (ret == REDIS_ERR) {
        printf("set callbackfailed\n");
        return -1;
    }

    ev_loop(EV_DEFAULT_ 0);

    return NULL;
}

void reply_callback(redisAsyncContext *c, void *reply, void *privdata)
{
    displayReply(reply);

}

int main(int argc, char **argv)
{
    redisAsyncContext *c = NULL;
    pthread_t tid;
    int ret =0;

    c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("Error: %s\n", c->errstr);
        return -1;
    }

    ret = pthread_create(&tid, NULL, test_libev_pthread, (void*)c);
    if (ret < 0) {
        printf("create pthread failed %s\n", strerror(errno));
        return -1;
    }

    redisAsyncCommand(c, reply_callback, NULL, "subscribe lilu");


    pthread_join(tid, NULL);

}


