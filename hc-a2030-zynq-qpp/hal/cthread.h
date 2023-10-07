#ifndef __CTHREAD_H__
#define __CTHREAD_H__
#include "../components/protocol/protcan.h"
#include "hprotcan.h"
#include "cgpio.h"
#define CMD_IDLE    0
#define CMD_BUSY    1
extern CHProtCan hpcan;
extern cgpio m_io;
extern int cmd_idle_flag;
extern int reboot_flag;
extern unsigned char *all_slot_recv_buf;
typedef struct
{
    uint8_t start;
    uint8_t data[12];
}wake_up_;

struct vol_cur{
    uint16_t vol;
    uint16_t cur;
};
struct chl_vol_cur{
    struct vol_cur vc_chl[5];
};

struct slot_vol_cur{
    struct chl_vol_cur vc_slot[10];
};

extern wake_up_ wake_up_t;


void cthread_init(void);
//private
void slot_device_online_check();

#endif
