#ifndef __PROTCAN_H
#define __PROTCAN_H


#ifdef __cplusplus
extern "C" {
#endif


#include <net/if.h>
#include <linux/can/raw.h>


#define PROTCAN_EXT_ID 1
#define CAN_FRAMES 32


typedef struct {
    unsigned int addr:16;
    unsigned int slot:4;
    unsigned int multi:1;
    unsigned int master:1;
    unsigned int ctrl:4;
    unsigned int :3;
    unsigned int err:1;
    unsigned int rtr:1;
    unsigned int eff:1;
} ext_id_t;

typedef struct {
    unsigned int data:11;
    unsigned int :18;
    unsigned int err:1;
    unsigned int rtr:1;
    unsigned int eff:1;
} std_id_t;

typedef union {
    unsigned int value;
    ext_id_t ext;
    std_id_t std;
} can_id_t;

typedef struct {
    int frames;
    struct can_frame frame[CAN_FRAMES];
} can_pack_t;

typedef struct {
    int bytes;
    unsigned char *buf;
    can_id_t id;
} can_stream_t;


class CProtCan
{
private:


public:
    CProtCan();
    ~CProtCan();

    void unpack(can_stream_t *strm, can_pack_t *pack, int dlc = 8);
    void packet(can_pack_t *pack, can_stream_t *strm, int dlc = 8);
    unsigned int genExtId(unsigned short addr, unsigned char slot, 
                bool multi, bool master, unsigned char ctrl, 
                bool err = false, bool rtr = false, bool eff = true);
    int isError(unsigned int id);
    int getCanFrames(int bytes, int dlc = 8);

};


#ifdef __cplusplus
}
#endif


#endif /* __PROTCAN_H */