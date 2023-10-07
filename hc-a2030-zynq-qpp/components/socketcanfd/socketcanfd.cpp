#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <sys/socket.h>

#include "socketcan.h"
#include "socketcanfd.h"


CSocketCanfd::CSocketCanfd()
{
    this->isOpen = false;
    this->handle = 0;
    memset(&this->addr, 0, sizeof(struct sockaddr_can));
    memset(&this->ifr, 0, sizeof(struct ifreq));
}

CSocketCanfd::~CSocketCanfd()
{
    if (isOpen == true) {
        this->closeDev();
    }
}

int CSocketCanfd::openDev(const char* dev)
{
    int ret = 0;
    if (true == this->isOpen) {
        return this->handle;
    }
    handle = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (handle < 0) {
        return -1;
    }
    strcpy(ifr.ifr_name, dev);
    ret = ioctl(handle, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        return -1;
    }
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(handle, (struct sockaddr *)&addr, 
        sizeof(struct sockaddr_can));
    if (ret < 0) {
        return -1;
    }
    this->isOpen = true;
    return handle;
}

int CSocketCanfd::closeDev(void)
{
    close(this->handle);
    this->handle = 0;
    return 0;
}

int CSocketCanfd::transmit(void *buf, int count)
{
    if (false == this->isOpen) {
        return -1;
    }
    return write(handle, buf, count);
}

int CSocketCanfd::receive(void *buf, int count)
{
    if (false == this->isOpen) {
        return -1;
    }
    return read(handle, buf, count);
}

int CSocketCanfd::setRecvTimo(int sec, int uSec)
{
    struct timeval timo = {
        .tv_sec = sec, 
        .tv_usec = uSec
    };
    return setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, 
            (const char *)&timo, sizeof(struct timeval));
}

int CSocketCanfd::clearRxBuf(void)
{
    int ret = 0;
    fd_set fds;
    char buf[4] = {0};
    struct timeval timo = {0, 0};

    FD_ZERO(&fds);
    FD_SET(this->handle, &fds);

    while (1) {
        ret = select(FD_SETSIZE, &fds, NULL, NULL, &timo);
        if (0 == ret) {
            break;
        }
        read(this->handle, buf, 1);
    }
    return 0;
}



