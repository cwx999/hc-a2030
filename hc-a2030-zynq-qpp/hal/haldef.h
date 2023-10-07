#ifndef __HALDEF_H
#define __HALDEF_H


// #ifdef __cplusplus
// extern "C" {
// #endif


#define FW_ARM_DATE    0x20230927
#define FW_ARM_VERSION 0x0000b104

#define CMD_VER                 0X0004
#define CMD_REBOOT              0X000C
#define CMD_ONLINE              0X0100
#define CMD_CHL_PWR             0X0200
#define CMD_SLOT_PWR            0X0202
#define CMD_REQ_SLOT_PWR        0X0203
#define CMD_WAKE_UP             0X0204
#define CMD_REQ_SLOT_VOL_CUR    0X0210
#define CMD_REQ_ALL_VOL_CUR     0X0224
#define CMD_WAKE_UP_START       0X02EC
#define CMD_CAL_VREF            0X0300
#define CMD_CAL_CUR             0X0304
#define CMD_SET_NET             0X0400
#define CMD_SET_PORT            0X040C

// #ifdef __cplusplus
// }
// #endif


#endif /* __HALDEF_H */
