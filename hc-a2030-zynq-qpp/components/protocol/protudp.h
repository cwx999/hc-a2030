#ifndef __PROTUDP_H
#define __PROTUDP_H


#ifdef __cplusplus
extern "C" {
#endif


#include "protocol.h"


#define MSG_HEAD_SIZE 6
#define MSG_DATA_SIZE 256
#define MSG_SIZE (MSG_HEAD_SIZE + MSG_DATA_SIZE)

/**
 * @brief 数据包开始、结束标识
 */
enum PACKET_CODE {
    HEAD_CODE = 0x55aa,
    TAIL_CODE = 0xaa55
};

/**
 * @brief 数据包头部信息
 * @param code 包头标识
 * @param size 数据包总大小，含CRC
 */
typedef struct {
    unsigned short code;
    unsigned short size;
} head_t;

/**
 * @brief 数据包内容
 * @param ctrl 控制字
 * @param slot 槽位号
 * @param addr 功能码
 * @param size buff中有效数据字节数
 * @param buff 数据内容
 * @note 字节流
 */
typedef struct {
    // struct {
    //     unsigned char rw:1;
    //     unsigned char :6;
    //     unsigned char err:1;
    // } ctrl;
    // unsigned char slot:4;
    // unsigned char :4;
    // unsigned short addr;
    // unsigned short size;
    // unsigned char  buff[MSG_SIZE];
    unsigned char buff[MSG_SIZE];
} message_t;

/**
 * @brief 数据包尾部信息
 * @param code 结束标识
 * @param checksum 数据包校验和
 */
typedef struct {
    unsigned short code;
    unsigned short checksum;
} tail_t;

/**
 * @brief 数据包定义
 * @param head 包头
 * @param msg 包内容
 * @param tail 包尾
 */
typedef struct {
    head_t head;
    message_t msg;
    tail_t tail;
} pack_t;

typedef enum {
    CTRL_WR = 0,
    CTRL_RD = 1
} ctrl_word_t;


class CProtUdp
{
public:
    CProtUdp();
    ~CProtUdp();

    void packet(unsigned char *buf, pack_t *pack);
    void unpack(pack_t *pack, unsigned char *buf);
    int isPackValid(unsigned char *buf);

private:
    unsigned short checksum16(unsigned char *buf, int count);
    unsigned int checksum32(void *buf, int count);
};


#ifdef __cplusplus
}
#endif


#endif /* __PROTUDP_H */
