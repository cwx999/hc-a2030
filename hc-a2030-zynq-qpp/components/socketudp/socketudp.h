#ifndef __SOCKETUDP_H
#define __SOCKETUDP_H


#ifdef __cplusplus
extern "C" {
#endif


#include <pthread.h>
#include <netinet/in.h>


class CSocketUdp
{
private:
    int socketFd;
    struct sockaddr_in local;
    struct sockaddr_in remote;
    pthread_mutex_t mutex;

public:
    CSocketUdp();
    ~CSocketUdp();

    int bindSocket(const char *address = "0.0.0.0", 
                    const unsigned short port = 8080);
    int closeSocket(void);

    int transmit(const void *buf, int count, 
                    const char *address = "0.0.0.0", 
                    const unsigned short port = 8080);
    int receive(void *buf, int count);

    int getSocketFd(void);
    int getRemoteAddress(char *address, unsigned short *port);
};


#ifdef __cplusplus
}
#endif


#endif /* __SOCKETUDP_H */
