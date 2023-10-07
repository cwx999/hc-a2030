#ifndef __HPROTUDP_H
#define __HPROTUDP_H
#include <stdint.h>

// #ifdef __cplusplus
// extern "C" {
// #endif


#include "../components/protocol/protocol.h"
#include "../components/protocol/protudp.h"
#include "../components/socketudp/socketudp.h"


typedef struct {
    union {
        struct {
            unsigned char rw:1;
            unsigned char :6;
            unsigned char err:1;
        } bit;
        unsigned char value;
    } ctrl;
    unsigned char slot:4;
    unsigned char :4;
    unsigned short addr;
    unsigned short size;
    unsigned char buff[MSG_DATA_SIZE];
} udp_msg_t;


class CHProtUdp : public CProtUdp
{
private:
    CSocketUdp udp;

public:
    CHProtUdp();
    ~CHProtUdp();

    int init(void);
    void msgPacket(unsigned char *buf, udp_msg_t *msg);
    void msgUnpack(udp_msg_t *msg, unsigned char *buf);
    // void packet(unsigned char *buf, pack_t *pack);
    // void unpack(pack_t *pack, unsigned char *buf);
    int isMsgVaild(unsigned char *buf);

    int transmit(pack_t *pack);
    int receive(pack_t *pack);
};


// #ifdef __cplusplus
// }
// #endif


#endif /* __HPROTUDP_H */
