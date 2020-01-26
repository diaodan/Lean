#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <errno.h>

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
    unsigned long   d_ino;
    off_t           d_off;
    unsigned short  d_reclen;
    char            d_name[];
};

#define BUF_SIZE    1024

int main(int argc, char **argv)
{
    int fd = -1;
    int ret = 0;

    fd = open(argv[1], O_RDONLY | O_DIRECTORY);
    if (fd < 0) {
        printf("open failed: %s\n", strerror(errno));
        return -errno;
    }

    ret = syscall(SYS_mknodat, fd, argv[2], S_IFREG, 0);
    if (ret < 0) {
        printf("syscall  failed: %s\n", strerror(errno));
        return -errno;
    }
    return 0;
}



