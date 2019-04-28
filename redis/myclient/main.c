#include "myhead.h"

int main(int argc, char **argv)
{
    int socketfd = -1;
    int ret = 0;

    socketfd = ll_redis_connect(argv[1], argv[2]);
    if (ret < 0) {
        return -1;
    }
    return 0;
}
