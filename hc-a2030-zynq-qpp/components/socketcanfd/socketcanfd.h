#ifndef __SOCKETCANFD_H
#define __SOCKETCANFD_H


#ifdef __cplusplus
extern "C" {
#endif


#include <net/if.h>
#include <linux/can/raw.h>


class CSocketCanfd
{
public:
    CSocketCanfd();
    ~CSocketCanfd();

    int openDev(const char* dev = "can0");
    int closeDev(void);

    int transmit(void *buf, int count);
    int receive(void *buf, int count);

    int setRecvTimo(int sec = 1, int uSec = 0);
    int clearRxBuf(void);

private:
    bool isOpen;
    int handle;
    struct sockaddr_can addr;
    struct ifreq ifr;
};


#ifdef __cplusplus
}
#endif


#endif /* __SOCKETCANFD_H */
