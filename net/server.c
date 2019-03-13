#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
    int sockfd, clientfd;
    struct sockaddr_in serveraddr;
    int port, ret;

    if (argc !=2) {
        printf("usage: a.out  port");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket failed %s\n", strerror(errno));
        exit(1);
    }

    port = atoi(argv[1]);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(0);

    int sock_opt = 1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt, sizeof(sock_opt));
    if (ret < 0) {
        printf("setsocketopt SO_REUSEADDR failed %s\n", strerror(errno));
        goto err;
    }

    ret = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0) {
        printf("bind failed\n");
        goto err;
    }

    listen(sockfd, 5);

    clientfd = accept(sockfd, NULL, NULL);
    if (clientfd < 0) {
        printf("accept failed\n");
        goto err;
    }

    shutdown(clientfd, SHUT_RD);
    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);

err:
    close(sockfd);
    return 0;
}
        


