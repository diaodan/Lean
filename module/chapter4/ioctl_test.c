#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define __USER__

#include "scull.h"

int main(int argc, char **argv)
{
    int ret;
    int fd = -1;
    int quantum = 0;

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("open %s failed, %s\n", argv[1], strerror(errno));
        return -errno;
    }

    ret = ioctl(fd, SCULL_IOCGQUANTUM, &quantum);
    if (ret < 0) {
        printf("ioctl failed %s\n", strerror(errno));
    }
    printf("quantum %d\n", quantum);

    ret = ioctl(fd, SCULL_IOCQQUANTUM, NULL);
    if (ret < 0) {
        printf("ioctl failed %s\n", strerror(errno));
    }
    printf("quantum %d\n", ret);
    return ret;
}
