#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in serveraddr;
    int port, ret;

    if (argc !=3) {
        printf("usage: a.out  ip port");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket failed%s\n", strerror(errno));
        exit(1);
    }

    port = atoi(argv[2]);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    ret = inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);
    if (ret !=1 ) {
        printf("inet_pton failed %s\n", strerror(errno));
        goto err;
    }

    ret = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret < 0) {
        printf("connect failed %s\n", strerror(errno));
        goto err;
    }
    
    shutdown(sockfd, SHUT_WR);
    

err:
    close(sockfd);
    return 0;
}
        


