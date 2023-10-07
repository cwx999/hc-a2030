#ifndef __HALPROT_H
#define __HALPROT_H


// #ifdef __cplusplus
// extern "C" {
// #endif


#include "haldef.h"
#include "hprotudp.h"
#include "hprotcan.h"

#define DATA_OFT    11


enum RdwrType {
    RdOnly = 0, 
    WrOnly
};

enum FuncCode {
    FwVersionDate = 0x0004, 
    DeviceReboot  = 0x000c, 
    SlotOnline    = 0x0100, 
    SetChanOnoff  = 0x0200, 
    SetSlotOnoff  = 0x0202, 
    GetSlotOnoff  = 0x0203, 
    WakeSlotDev   = 0x0204, 
    GetSlotVolCur = 0x0210, 
    GetChanVolCur = 0x0224,
    SetRefVol   =0x300,
    
};

typedef struct {
    unsigned short addr;
    unsigned char rw;
    int (*func)(udp_msg_t *msg, udp_msg_t *ack);
} func_map_t;


extern CHProtUdp hpudp;

extern func_map_t func_list[];

int halProtocolHandle(func_map_t *list);

int getFwVersionDate(udp_msg_t *master, udp_msg_t *slave);



// #ifdef __cplusplus
// }
// #endif


#endif /* __HALPROT_H */
