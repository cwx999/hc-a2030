#ifndef __SOCKETTCP_H
#define __SOCKETTCP_H


#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <netinet/in.h>

class CSocketTcp
{
public:
    typedef enum {
        ServerMode = 0,
        ClientMode
    } WorkMode;

private:
    int socketFd;
    int clientFd;
    struct sockaddr_in server;
    struct sockaddr_in client;

    int workMode;
    bool isConn;
    pthread_mutex_t mutex;
    
public:
    CSocketTcp(CSocketTcp::WorkMode mode = ServerMode);
    ~CSocketTcp();

    int create(const char *address = "0.0.0.0", 
                    const unsigned short port = 8080, 
                    bool reuseaddr = true);
    int waitClient(void);

    int connectTo(const char *address = "127.0.0.1", 
                    const unsigned short port = 8080);
    int closeClient(void);
    int closeSocket(void);
    
    int transmit(const void *buf, int count);
    int receive(void *buf, int count);

    int getSocketFd(void);
    int getClientFd(void);
    int setSocketTimeout(int tv_sec = 1, int tv_usec = 0);
    int setClientTimeout(int tv_sec = 1, int tv_usec = 0);
    int setKeepAlive(int idle = 1, int interval = 1, int count = 3);
    bool isAlive(void);
    bool isServer(void);
    bool isConnected(void);
};


#ifdef __cplusplus
}
#endif


#endif /* __SOCKETTCP_H */
