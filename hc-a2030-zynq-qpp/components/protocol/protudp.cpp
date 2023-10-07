#include <stdio.h>
#include <string.h>

#include "protudp.h"
#include "../utils/utils.h"



CProtUdp::CProtUdp()
{}

CProtUdp::~CProtUdp()
{}

void CProtUdp::packet(unsigned char *buf, pack_t *pack)
{
    u16_to_u8buf(buf, pack->head.code);
    u16_to_u8buf(buf + 2, pack->head.size);
    memcpy(buf + 4, pack->msg.buff, pack->head.size - 8);
    u16_to_u8buf(buf + pack->head.size - 4, pack->tail.code);
    u16_to_u8buf(buf + pack->head.size - 2, pack->tail.checksum);
}

void CProtUdp::unpack(pack_t *pack, unsigned char *buf)
{
    pack->head.code = u8buf_to_u16(buf);
    pack->head.size = u8buf_to_u16(buf + 2);
    memcpy(pack->msg.buff, buf + 4, pack->head.size - 8);
    pack->tail.code = u8buf_to_u16(buf + pack->head.size - 4);
    pack->tail.checksum = u8buf_to_u16(buf + pack->head.size - 2);
}

int CProtUdp::isPackValid(unsigned char *buf)
{
    unsigned short pack_size = 0;
    unsigned short sum1 = 0, sum2 = 0;
    if (HEAD_CODE != u8buf_to_u16(buf)) {
        printf("HEAD_CODE\n");
        return -EPACKET;
    }
    pack_size = u8buf_to_u16(buf + 2);
    if ((pack_size > (MSG_SIZE + 8)) || (pack_size < 8)) {
        printf("EDATASIZE\n");
        return -EDATASIZE;
    }
    if (TAIL_CODE != u8buf_to_u16(buf + pack_size - 4)) {
        printf("TAIL_CODE\n");
        return -EPACKET;
    }
    sum1 = u8buf_to_u16(buf + pack_size - 2);
    sum2 = checksum16(buf, pack_size - 2);
    if (sum1 != sum2) {
        printf("ECHKSUM\n");
        return -ECHKSUM;
    }
    return ENONE;
}

unsigned short CProtUdp::checksum16(unsigned char *buf, int count)
{
    unsigned short i = 0, sum = 0;
    for (i = 0; i < count; i++) {
        sum += buf[i];
    }
    return sum;
}

unsigned int CProtUdp::checksum32(void *buf, int count)
{
    return 0;
}

