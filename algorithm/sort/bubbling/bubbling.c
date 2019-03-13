#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void sort(int *v, int n)
{
    int i, j;
    int tmp;
    for(i = 0; i < n; i ++) {
        for (j = 0; j < n - i - 1; j ++) {
            if (v[j] < v[j +1]) {
                tmp = v[j+1];
                v[j+1] = v[j];
                v[j] = tmp;
            }
        }
    }
    return ;
}

int main(int argc, char **argv)
{
    int fd = -1;
    void *buff = NULL, *p = NULL;
    int size = 0;
    int left = 0;
    int i = 0;
    int ret = 0;

    if (argc == 3) {
        fd = open(argv[2], O_RDONLY);
        if (fd < 0) {
            printf("open failed %s %s\n", argv[2], strerror(errno));
            exit(-1);
        }
    }

    size = atoi(argv[1]);

    buff = malloc(sizeof(int) * size);
    if (buff == NULL) {
        fprintf(stderr, "malloc failed %s\n", strerror(errno));
        goto failed;
    }

    if (fd != -1) {
        left = size * sizeof(int);
        p = buff;
        while (left) {
            ret = read(fd, p, left);
            if (ret < 0) {
                fprintf(stderr, "read failed %s\n", strerror(errno));
                goto failed;
            } else if (ret == 0) {
                fprintf(stderr, "end of filed size %d item %d\n", size, (size - left)/sizeof(int));
                goto failed;
            } else {
                left -= ret;
                p += ret;
            }
        }
    }
    sort(buff, size);
/*
    for (i = 1; i <= size; i++) {
        printf("%d ", ((int *)buff)[i-1]);
        if (i%20 == 0) {
            printf("\n");
        }
    }
    printf("\n");
*/

failed:
    if (buff) {
        free(buff);
    }
    close(fd);
    return 0;
}




        


                
