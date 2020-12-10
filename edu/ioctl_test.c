#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define __USER__

#include "edu.h"

int main(int argc, char **argv)
{
    int ret;
    int fd = -1;

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("open %s failed, %s\n", argv[1], strerror(errno));
        return -errno;
    }

    ret = ioctl(fd, EDU_IOCECHO, NULL);
    if (ret < 0) {
        printf("ioctl failed %s\n", strerror(errno));
    }

    return ret;
}
