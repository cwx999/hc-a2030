#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include "../inc/sockettcp.h"

CSocketTcp::CSocketTcp(CSocketTcp::WorkMode mode)
{
    socketFd = 0;
    clientFd = 0;
    isConn = false;
    workMode = mode;
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));

    pthread_mutex_init(&mutex, NULL);
}

CSocketTcp::~CSocketTcp()
{
    pthread_mutex_destroy(&mutex);
    closeClient();
    closeSocket();
}


int CSocketTcp::create(const char *address, 
                const unsigned short port, 
                bool reuseaddr)
{
    int ret = 0;

    if (true == isConn) {
        return -1;
    }
    if (false == this->isServer()) {
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(address);

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        perror("socket():");
        return ret;
    }
    /**
     * @brief 端口重用
     */
    int on = reuseaddr;
    ret = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, 
                        &on, sizeof(int));
    if (ret < 0) {
        return ret;
    }
    ret = bind(socketFd, (struct sockaddr *)&server, 
                sizeof(struct sockaddr_in));
    if (ret < 0) {
        this->closeSocket();
        perror("bind():");
        return ret;
    }
    listen(socketFd, 5);
    return 0;
}

int CSocketTcp::waitClient(void)
{
    socklen_t length = sizeof(struct sockaddr_in);
    char strIp[32] = {0};
    if (true == isConn) {
        return -1;
    }
    if (false == this->isServer()) {
        return -1;
    }
    clientFd = accept(socketFd, (struct sockaddr *)&client, &length);
    if (clientFd < 0) {
        // perror("accept():");
        return clientFd;
    }
    inet_ntop(AF_INET, &client.sin_addr, strIp, sizeof(strIp));
    printf("Client %s::%d connected\n", strIp, ntohs(client.sin_port));
    isConn = true;
    return clientFd;
}

int CSocketTcp::connectTo(const char *address, const unsigned short port)
{
    int ret = 0;

    if (true == isConn) {
        printf("conn\n");
        return -1;
    }
    if (true == this->isServer()) {
        printf("server mode\n");
        return -1;
    }

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(address);

    clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        perror("socket():");
        return -1;
    }

    ret = connect(clientFd, (struct sockaddr *)&client, sizeof(struct sockaddr_in));
    if (ret < 0) {
        perror("connect():");
        closeClient();
        return ret;
    }
    isConn = true;
    printf("Connected to server %s::%d\n", address, port);

    return clientFd;
}

int CSocketTcp::closeClient(void)
{
    if (this->clientFd > 0) {
        close(clientFd);
    }
    clientFd = 0;
    isConn = false;
    return 0;
}

int CSocketTcp::closeSocket(void)
{
    if (this->socketFd > 0) {
        close(socketFd);
    }
    socketFd = 0;
    return 0;
}

int CSocketTcp::transmit(const void *buf, int count)
{
    int ret = 0;

    if (false == isConn) {
        return -1;
    }
    pthread_mutex_lock(&this->mutex);
    ret = send(clientFd, buf, count, 0);
    pthread_mutex_unlock(&this->mutex);
    // printf("ret: %d, errno: %d\n", ret, errno);
    if (ret <= 0 ) {
        perror("transmit");
    }
    return ret;
}

int CSocketTcp::receive(void *buf, int count)
{
    int ret = 0;

    if (false == isConn) {
        return -1;
    }
    ret = recv(clientFd, buf, count, 0);
    if (ret <= 0) {
        perror("receive");
    }
    return ret;
}

int CSocketTcp::getSocketFd(void)
{
    return socketFd;
}

int CSocketTcp::getClientFd(void)
{
    return clientFd;
}


int CSocketTcp::setSocketTimeout(int tv_sec, int tv_usec)
{
    struct timeval timeo = {
        .tv_sec = tv_sec, 
        .tv_usec = tv_usec
    };
    return setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, 
                (const char *)&timeo, sizeof(struct timeval));
}

int CSocketTcp::setClientTimeout(int tv_sec, int tv_usec)
{
    struct timeval timeo = {
        .tv_sec = tv_sec, 
        .tv_usec = tv_usec
    };
    return setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, 
                (const char *)&timeo, sizeof(struct timeval));
}



bool CSocketTcp::isServer(void)
{
    if (this->workMode == WorkMode::ServerMode) {
        return true;
    }
    return false;
}

bool CSocketTcp::isConnected(void)
{
    struct tcp_info info;
    int length = sizeof(struct tcp_info);
    getsockopt(clientFd, IPPROTO_TCP, 
                TCP_INFO, &info, (socklen_t *)&length);
    printf("state:%d\n", info.tcpi_state);
    if (TCP_ESTABLISHED == info.tcpi_state) {
        printf("connect: TCP_ESTABLISHED\n");
        return true;
    }
    printf("connect: other\n");
    return false;
}



