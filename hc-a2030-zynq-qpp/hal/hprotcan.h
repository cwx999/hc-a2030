#ifndef __HPROTCAN_H
#define __HPROTCAN_H


// #ifdef __cplusplus
// extern "C" {
// #endif


#include <pthread.h>
#include <string>
#include "../components/protocol/protocol.h"
#include "../components/protocol/protcan.h"
#include "../components/socketcan/socketcan.h"

class CHProtCan : public CProtCan
{
private:
    CSocketCan can;
    pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
public:
    CHProtCan();
    ~CHProtCan();

    int init(void);
    int init(std::string strDev);
    int transmit(can_pack_t *pack);
    int receive(can_pack_t *pack);
    int forward(can_stream_t *strm, can_stream_t *ack);

};

class CHPortCanFd
{
public:
    CHPortCanFd();
    ~CHPortCanFd();
    int init(void);
    int uninit(void);
    int transmit(int slot, void *buf, int count);
private:
    CSocketCan canfd[10];
};

// #ifdef __cplusplus
// }
// #endif


#endif /* __HPROTCAN_H */
