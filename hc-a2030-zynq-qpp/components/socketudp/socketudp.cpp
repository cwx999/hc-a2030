#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "socketudp.h"


CSocketUdp::CSocketUdp()
{
    socketFd = 0;
    memset(&local, 0, sizeof(struct sockaddr_in));
    memset(&remote, 0, sizeof(struct sockaddr_in));
}

CSocketUdp::~CSocketUdp()
{
    closeSocket();
}

int CSocketUdp::bindSocket(const char *address, 
                const unsigned short port)
{
    int ret = 0;

    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(address);

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0) {
        return -1;
    }

    ret = bind(socketFd, (struct sockaddr *)&local, sizeof(local));
    if (ret < 0) {
        return -1;
    }

    return socketFd;
}

int CSocketUdp::closeSocket(void)
{
    if (socketFd > 0) {
        close(socketFd);
    }
    socketFd = 0;
    return 0;
}

int CSocketUdp::transmit(const void *buf, int count, 
                const char *address, const unsigned short port)
{
    if (0 != strlen(address)) {
        remote.sin_family = AF_INET;
        remote.sin_port = htons(port);
        remote.sin_addr.s_addr = inet_addr(address);
    }
    return sendto(socketFd, buf, count, 0, 
                (struct sockaddr *)&remote, sizeof(struct sockaddr_in));
}

int CSocketUdp::receive(void *buf, int count)
{
    socklen_t length = sizeof(struct sockaddr_in);
    return recvfrom(socketFd, buf, count, 0, 
                (struct sockaddr *)&remote, &length);
}

int CSocketUdp::getSocketFd(void)
{
    return socketFd;
}
