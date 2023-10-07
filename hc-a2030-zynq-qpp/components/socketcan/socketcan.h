#ifndef __SOCKETCAN_H
#define __SOCKETCAN_H


#ifdef __cplusplus
extern "C" {
#endif


#include <net/if.h>
#include <linux/can/raw.h>


class CSocketCan
{
public:
    CSocketCan();
    ~CSocketCan();

    int openDev(const char* dev = "can0", bool canfdOn = false);
    int closeDev(void);

    int transmit(void *buf, int count);
    int receive(void *buf, int count);

    int setRecvTimo(int sec = 1, int uSec = 0);
    int setSendTimo(int sec = 1, int uSec = 0);
    int clearRxBuf(void);
    int clearTxBuf(void);
    int disRecvfilter(void);
    int SetNonBlock(void);

private:
    bool isOpen;
    int handle;
    struct sockaddr_can addr;
    struct ifreq ifr;
};


#ifdef __cplusplus
}
#endif


#endif
