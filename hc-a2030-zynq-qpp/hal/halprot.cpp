#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <string>
#include <iostream>
#include "halprot.h"
#include "haldef.h"
#include "cgpio.h"
#include "../components/utils/utils.h"
#include "iodef.h"
#include "cthread.h"
#include "cnetcfg.h"

CHProtUdp hpudp;


int getFwVersionDate(udp_msg_t *master, udp_msg_t *slave);
int setFwReboot(udp_msg_t *master, udp_msg_t *slave);
int getSlotStatus(udp_msg_t *master, udp_msg_t *slave);
int setChlPower(udp_msg_t *master, udp_msg_t *slave);
int setSlotPower(udp_msg_t *master, udp_msg_t *slave);
int getSlotPower(udp_msg_t *master, udp_msg_t *slave);
int setHwWakeUp(udp_msg_t *master, udp_msg_t *slave);
int setHwWakeUpStart(udp_msg_t *master, udp_msg_t *slave);
int getSlotVolCur(udp_msg_t *master, udp_msg_t *slave);
int getAllVolCur(udp_msg_t *master, udp_msg_t *slave);
int setCalVref(udp_msg_t *master, udp_msg_t *slave); 
int setCalCur(udp_msg_t *master, udp_msg_t *slave);
int setnet(udp_msg_t *master, udp_msg_t *slave);
int setport(udp_msg_t *master, udp_msg_t *slave);

func_map_t func_list[] = {
    {CMD_VER, RdOnly, getFwVersionDate}, 
    {CMD_REBOOT, WrOnly, setFwReboot},
    {CMD_ONLINE, RdOnly, getSlotStatus},
    {CMD_CHL_PWR, WrOnly, setChlPower},
    {CMD_SLOT_PWR, WrOnly, setSlotPower},
    {CMD_REQ_SLOT_PWR, RdOnly, getSlotPower},
    {CMD_WAKE_UP, WrOnly, setHwWakeUp},
    {CMD_REQ_SLOT_VOL_CUR, RdOnly,getSlotVolCur},
    {CMD_REQ_ALL_VOL_CUR, RdOnly,getAllVolCur},
    {CMD_WAKE_UP_START, WrOnly, setHwWakeUpStart},
    {CMD_CAL_VREF, WrOnly, setCalVref},
    {CMD_CAL_CUR, WrOnly, setCalCur},
    {CMD_SET_NET, WrOnly, setnet},
    {CMD_SET_PORT, WrOnly, setport},
    {0, RdOnly, nullptr}
};

int halProtMsgHandle(func_map_t *list, pack_t *master, pack_t *slave)
{
    // int ret = 0;
    udp_msg_t m = {0}, s = {0};
    func_map_t *ptr = nullptr;
    unsigned short errCode = 0;
    hpudp.msgUnpack(&m, master->msg.buff);
    memcpy(&s, &m, 6);
    // show_hexdump(&m, 0, 8);
    do {
        for (ptr = list; nullptr != ptr->func; ptr++) {
            if (m.addr != ptr->addr) {
                continue;
            }
            break;
        }
        if (nullptr == ptr->func) {
            errCode = ENOADDR;
            break;
        }
        errCode = ptr->func(&m, &s);
        if (errCode < 0) {
            break;
        }
        hpudp.msgPacket(slave->msg.buff, &s);
        return 0;
    } while (0);
    s.ctrl.bit.err = 1;
    s.size = 2;
    u16_to_u8buf(s.buff, errCode);
    hpudp.msgPacket(slave->msg.buff, &s);
    return -1;
}

int halProtocolHandle(func_map_t *list)
{
    int ret = 0;
    pack_t m = {0}, s = {0};
    if (hpudp.receive(&m) < 0) {
        return -1;
    }

    ret = halProtMsgHandle(list, &m, &s);
    if (ret < 0) {
        
    }

    hpudp.transmit(&s);
    return 0;
}

int getFwVersionDate(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    can_stream_t m = {0}, s = {0};
    slave->size = 8;
    if (0 == master->slot) {
        u32_to_u8buf(slave->buff, FW_ARM_DATE);
        u32_to_u8buf(slave->buff + 4, FW_ARM_VERSION);
    }
    else {
        cmd_idle_flag = CMD_BUSY;
        m.buf = master->buff;
        m.bytes = master->size;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        ret = hpcan.forward(&m, &s);
        cmd_idle_flag = CMD_IDLE;
    }
    return ret;
}

int setFwReboot(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    can_stream_t m = {0}, s = {0};
    pack_t p_t;
    slave->size = 2;
    uint16_t flag = 0;
    if (0 == master->slot) {
        
        flag = u8buf_to_u16(master->buff);
        if (flag) {
            hpudp.msgPacket(p_t.msg.buff, slave);
            hpudp.transmit(&p_t);
            usleep(1000000);
            sync();
            reboot(RB_AUTOBOOT);
            
        }

    }
    else {
        do 
        {
            usleep(100);
        }while(cmd_idle_flag != CMD_IDLE );
        cmd_idle_flag = CMD_BUSY;
        m.buf = master->buff;
        m.bytes = master->size;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        ret = hpcan.forward(&m, &s);
        reboot_flag = 1;
        cmd_idle_flag = CMD_IDLE;
        
        
    }
    return ret;
}

int getSlotStatus(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    uint16_t stat = 0;
    slave->size = 2;
    if (0 == master->slot) {
        stat = m_io.get_insert_stat();
        printf("slot stat:%x\n", stat);
        u16_to_u8buf(slave->buff, stat);
    }
    else {
        ret = ENOSLOT;
    }
    return ret;
}

int setChlPower(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    can_stream_t m = {0}, s = {0};
    slave->size = 2;
    if (0 == master->slot) {
        ret = ENOSLOT;
    }
    else {
        cmd_idle_flag = CMD_BUSY;
        m.buf = master->buff;
        m.bytes = 2;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        printf("value:%x\n", m.id.value);
        ret = hpcan.forward(&m, &s);
        cmd_idle_flag = CMD_IDLE;
    }
    return ret;
}

int setSlotPower(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    can_stream_t m = {0}, s = {0};
    slave->size = 1;
    if (0 == master->slot) {
        ret = ENOSLOT;
    }
    else {
        cmd_idle_flag = CMD_BUSY;
        m.buf = master->buff;
        m.bytes = master->size;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        ret = hpcan.forward(&m, &s);
        cmd_idle_flag = CMD_IDLE;
    }
    return ret;

}

int getSlotPower(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    can_stream_t m = {0}, s = {0};
    slave->size = 1;
    if (0 == master->slot) {
        ret = ENOSLOT;
    }
    else {
        cmd_idle_flag = CMD_BUSY;
        m.buf = master->buff;
        m.bytes = master->size;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        ret = hpcan.forward(&m, &s);
        cmd_idle_flag = CMD_IDLE;
    }
    return ret;
}

int setHwWakeUp(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    slave->size = 1;
    // if (0 == master->slot) {
    //      ret = ENOSLOT;
    // }
    // else {
        memset(&wake_up_t, 0, sizeof(wake_up_t));
        memcpy(wake_up_t.data, master->buff, master->size);
    // }
    return ret;    
}

int setHwWakeUpStart(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    slave->size = 1;
    // if (0 == master->slot) {
    //      ret = ENOSLOT;
    // }
    // else {
        wake_up_t.start = master->buff[0];
        slave->size = 1;
    // }
    return ret;    
}

int getSlotVolCur(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    can_stream_t m = {0}, s = {0};
    slave->size = 20;
    uint16_t vol, cur;
    int index = 0;
    if (0 == master->slot) {
        ret = ENOSLOT;
    }
    else {
        cmd_idle_flag = CMD_BUSY;
        m.buf = master->buff;
        m.bytes = master->size;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        ret = hpcan.forward(&m, &s);

        for (size_t i = 0; i < 5; i++)
        {
                vol = u8buf_to_u16(slave->buff + index);index +=2;
                cur = u8buf_to_u16(slave->buff + index);index+=2;
                printf ("chl[%d]-vol[%d]-cur[%d]\n",i+1, vol, cur);
        }
        cmd_idle_flag = CMD_IDLE;
    }
    return ret;
}

int getAllVolCur(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    #if 0
    int i = 0;
    can_stream_t m = {0}, s = {0};
    unsigned char recv_buf[20] = {0};
    int index = 0;
    int iostat = 0;
    uint16_t vol, cur;
    slave->size = 200;
    m.bytes = 20;
    m.buf = recv_buf;

    
    if (0 == master->slot) 
    {

        iostat = m_io.get_insert_stat();
        for (i = 0; i < 10; i++)
        {
            if (((iostat >> i) & 0x1) == 1)
            {
                m.id.value = hpcan.genExtId(0x210,i+1, false, true,0);
                s.buf = recv_buf;
                ret = hpcan.forward(&m, &s);
                memcpy(slave->buff + i * 20, recv_buf, 20);
                memset(recv_buf, 0, 20);
            }

        }
        for (size_t i = 0; i < 50; i++)
        {
                vol = u8buf_to_u16(slave->buff + index);index +=2;
                cur = u8buf_to_u16(slave->buff + index);index+=2;
                printf ("chl[%d]-vol[%d]-cur[%d]\n",i+1, vol, cur);
        }

        ///TODO::所有通道电压电流查询
    }
    else {
        ret = ENOSLOT;
    }
    #else
    slave->size = 200;
    memcpy(slave->buff, all_slot_recv_buf, 200);
    #endif
    return ret;
}

int setCalVref(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    can_stream_t m = {0}, s = {0};
    
    slave->size = 2;
    printf("%d\n",__LINE__);
    if (0 == master->slot) {
        ret = ENOSLOT;
    }
    else {
        cmd_idle_flag = CMD_BUSY;
        m.buf = master->buff;
        m.bytes = master->size;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        ret = hpcan.forward(&m, &s);

        cmd_idle_flag = CMD_IDLE;
    }
    return ret;

}

int setCalCur(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    
    can_stream_t m = {0}, s = {0};
    slave->size = 1;
    if (0 == master->slot) {
        ret = ENOSLOT;
    }
    else {
        cmd_idle_flag = CMD_BUSY;
        printf("%d\n", __LINE__);
        m.buf = master->buff;
        m.bytes = 1;
        m.id.value = hpcan.genExtId(master->addr, master->slot, 
                    false, true, master->ctrl.value);
        s.buf = slave->buff;
        ret = hpcan.forward(&m, &s);
        cmd_idle_flag = CMD_IDLE;
    }
    return ret;
}

int setnet(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    std::string str_ip, str_mask, str_gateway;
    for (int i = 0; i < 4; i++)
    {
        str_ip.append(std::to_string(master->buff[i]));
        str_mask.append(std::to_string(master->buff[i+4]));
        str_gateway.append(std::to_string(master->buff[i+8]));
        if (i < 3)
        {
            str_ip.append(".");
            str_mask.append(".");
            str_gateway.append(".");
        }
        
    }
    printf("%d\n",__LINE__);
    std::cout<<"ip:"<<str_ip<<"\n";
    std::cout<<"mask:"<<str_mask<<"\n";
    std::cout<<"gateway:"<<str_gateway<<"\n";

    setNet(str_ip, str_mask, str_gateway);

    slave->size = 1;

    return ret;
}

int setport(udp_msg_t *master, udp_msg_t *slave)
{
    int ret = 0;
    std::string str_port;
    uint16_t port = master->buff[0] << 8 |
                    master->buff[1];
    str_port = std::to_string(port);

    printf("%d\n",__LINE__);
    std::cout<<"port:"<<str_port<<"\n";
    setPort(str_port);
    slave->size = 1;

    return ret;
}



