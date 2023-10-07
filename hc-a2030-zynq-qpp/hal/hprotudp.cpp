#include <stdio.h>
#include <string.h>

#include "hprotudp.h"
#include "../components/utils/utils.h"
#include "cnetcfg.h"

CHProtUdp::CHProtUdp() {}

CHProtUdp::~CHProtUdp()
{
    udp.closeSocket();
}

int CHProtUdp::init(void)
{
    int ret = 0;
    ret = udp.bindSocket("0.0.0.0", getPort());
    if (ret < 0) {
        return -1;
    }
    return 0;
}

void CHProtUdp::msgPacket(unsigned char *buf, udp_msg_t *msg)
{
    buf[0] = msg->ctrl.value;
    buf[1] = msg->slot;
    u16_to_u8buf(buf + 2, msg->addr);
    u16_to_u8buf(buf + 4, msg->size);
    memcpy(buf + 6, msg->buff, msg->size);
}

void CHProtUdp::msgUnpack(udp_msg_t *msg, unsigned char *buf)
{
    msg->ctrl.value = buf[0];
    msg->slot = buf[1];
    msg->addr = u8buf_to_u16(buf + 2);
    msg->size = u8buf_to_u16(buf + 4);
    memcpy(msg->buff, buf + 6, msg->size);
}

int CHProtUdp::isMsgVaild(unsigned char *buf)
{
    if (u8buf_to_u16(buf + 4) > MSG_DATA_SIZE) {
        return -EDATASIZE;
    }
    return ENONE;
}

int CHProtUdp::transmit(pack_t *pack)
{
    unsigned char buf[600] = {0};
    pack->head.code = HEAD_CODE;
    pack->head.size = u8buf_to_u16(pack->msg.buff + 4) + 6 + 8;
    pack->tail.code = TAIL_CODE;
    packet(buf, pack);
    for (short i = 0; i < (pack->head.size - 2); i++) {
        pack->tail.checksum += buf[i];
    }
    u16_to_u8buf(buf + pack->head.size - 2, pack->tail.checksum);

    // printf("***************** ack **********************\n");
    // show_hexdump(buf, 0, 64);

    return udp.transmit(buf, pack->head.size, "", 0);
}

int CHProtUdp::receive(pack_t *pack)
{
    int ret = 0;
    unsigned char buf[600] = {0};
    ret = udp.receive(buf, sizeof(buf));
    if (ret <= 0) {
        return -1;
    }

    // printf("***************** cmd **********************\n");
    // show_hexdump(buf, 0, 64);

    if (isPackValid(buf) < 0) {
        return -1;
    }
    unpack(pack, buf);
    return ret;
}

