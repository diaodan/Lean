#include "mynet.h"
#include "ll_log.h"

int ll_redis_connect(const char *host_ip, const char *host_port)
{
    int fd = -1;
    struct sockaddr_in serveraddr;
    uint16_t port;
    int ret;

    bzero(&serveraddr, sizeof(struct sockaddr_in));
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        ERROR("open socket failed: %s\n", strerror(errno));
        return -errno;
    }
    port = (uint16_t)atoi(host_port);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htonl(port);
    ret = inet_pton(AF_INET, host_ip, &serveraddr.sin_addr);
    if (ret != 1) {
        if (ret == 0) {
            ERROR("inet pton failed: invalid ip %s\n", host_ip);
        } else {
            ERROR("inet pton failed: %s\n", strerror(errno));
        }
        goto err;
    }

    ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        ERROR("connect failed: %s\n", strerror(errno));
        goto err;
    }

    return fd;

  err:
    close(fd);
    return -errno;
}
