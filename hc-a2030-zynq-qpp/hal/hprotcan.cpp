#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "hprotcan.h"
#include <linux/can.h>
#include <sys/socket.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>
#include "../components/utils/utils.h"

CHProtCan::CHProtCan()
{}

CHProtCan::~CHProtCan()
{
    can.closeDev();
}

int CHProtCan::init(void)
{
    int ret = 0;
    ret = can.openDev("can0");
    if (ret < 0) {
        perror("open error");
        return ret;
    }
    can.setSendTimo(1,0);
    return can.setRecvTimo(1, 0);
}

int CHProtCan::init(std::string strDev)
{
    int ret = 0;
    ret = can.openDev(strDev.c_str());
    if (ret < 0) {
        perror("open error");
        return ret;
    }
    can.SetNonBlock();
    return 0;
}

int CHProtCan::transmit(can_pack_t *pack)
{
    int ret = 0;
    for (int i = 0; i < pack->frames; i++) {
        ret = can.transmit(pack->frame + i, sizeof(struct can_frame));
        if (ret <= 0) {
            // printf("line %d--ret = %d\n",__LINE__, ret);
            return -EOPERAT;
        }
    }
    return 0;
}

int CHProtCan::receive(can_pack_t *pack)
{
    int ret = 0, i = 0;
    can_id_t lastId, currId;
    for (i = 0; i < CAN_FRAMES; i++) {
        ret = can.receive(pack->frame + i, sizeof(struct can_frame));
       // show_hexdump(pack->frame + i,0,sizeof(struct can_frame));
        if (ret <= 0) {
            // perror("time_out\n");
            return -1;
        }
        // 检查是否是从机应答
        currId.value = pack->frame[i].can_id;
        if (currId.ext.master) {
            continue;
        }
        // 检查帧的连续性
        if (i) {
            if (lastId.ext.slot != currId.ext.slot) {
                return -1;
            }
            if (8 != (currId.ext.addr - lastId.ext.addr)) {
                return -1;
            }
        }
        pack->frames++;
        lastId = currId;
        if (!currId.ext.multi) {
            return 0;
        }
    }
    // 结束帧检查
    if (currId.ext.multi && (32 == i)) {
        return -1;
    }
    return 0;
}

int CHProtCan::forward(can_stream_t *strm, can_stream_t *ack)
{
    // int ret = 0;
    pthread_mutex_lock(&gMutex);
    can_pack_t master = {0}, slave = {0};
    packet(&master, strm);
    can.clearRxBuf();
    if (transmit(&master) < 0) {
        pthread_mutex_unlock(&gMutex);
        return -ETRANSFER;
    }
    if (receive(&slave) < 0) {
        pthread_mutex_unlock(&gMutex);
        return -ETRANSFER;
    }
    if (isError(slave.frame[0].can_id) < 0) {
        pthread_mutex_unlock(&gMutex);
        return isError(slave.frame[0].can_id);
    }
    unpack(ack, &slave);
    pthread_mutex_unlock(&gMutex);
    return 0;
}

CHPortCanFd::CHPortCanFd()
{
    init();
}

CHPortCanFd::~CHPortCanFd()
{
    uninit();
}

int CHPortCanFd::init(void)
{
    int ret = 0;
    int count = 10;
    char strDev[20] = {0};
    for (size_t i = 0; i < count; i++)
    {
        memset(strDev, 0, sizeof(strDev));
        sprintf(strDev, "can%d", i+1);
        ret = canfd[i].openDev(strDev, false);
        if (ret < 0) 
        {
            perror("open error");
            return ret;
        }
        // canfd[i].setSendTimo(1,0);
        canfd[i].disRecvfilter();
        canfd[i].SetNonBlock();
    
    }
    
    return 0;
}

int CHPortCanFd::uninit(void)
{
    for (size_t i = 0; i < 10; i++)
    {
        canfd[i].closeDev();
    }
    return 0;
    
}

int CHPortCanFd::transmit(int slot, void *buf, int count)
{
    int ret = 0;
    int state;
    uint32_t id = 0;
    // struct canfd_frame i_frame;
    struct can_frame i_frame;
    
    id = u8buf_to_u32((unsigned char*)buf);
    memcpy(&i_frame.can_id, &id, sizeof(int));
    i_frame.can_dlc = count-4;

    memcpy(i_frame.data, (char*)buf+4, i_frame.can_dlc);

    // printf("%X\n", i_frame.can_id);

    ret = canfd[slot].transmit(&i_frame,sizeof(can_frame));
    if (ret <= 0) 
    {
        perror("write error\n");
        // printf("errno = %x\n", errno);
        canfd[slot].clearTxBuf();
        // printf("line %d ret = %d\n", __LINE__, ret);
        // char strDev[20] = {0};
        // sprintf(strDev, "can%d", slot+1);
        // canfd[slot].clearRxBuf();
        // return ret;
    }
    // usleep(10000);
    

    return ret;
}





