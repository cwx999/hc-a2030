#ifndef __UTILS_H
#define __UTILS_H


// #ifdef __cplusplus
// extern "C" {
// #endif

#ifndef NULL
#define NULL (void *)0
#endif

// #define min(a, b)  (((a) < (b)) ? (a) : (b))
// #define max(a, b)  (((a) > (b)) ? (a) : (b))

#define endian_swap16(num) \
        ((((unsigned short)num & 0xff00) >> 8) | \
        (((unsigned short)num & 0x00ff) << 8) )

#define endian_swap32(num) \
        ((((unsigned int)num & 0xff000000) >> 24 ) | \
        (((unsigned int)num & 0x00ff0000) >>  8 ) | \
        (((unsigned int)num & 0x0000ff00) <<  8 ) | \
        (((unsigned int)num & 0x000000ff) << 24 ) )

unsigned short u8buf_to_u16(unsigned char *src);
void u16_to_u8buf(unsigned char *dest, unsigned short src);
unsigned int u8buf_to_u32(unsigned char *src);
void u32_to_u8buf(unsigned char *dest, unsigned int src);


int is_little_endian(void);
int is_big_endian(void);
unsigned short mhtons(unsigned short value);
unsigned short mntohs(unsigned short value);
unsigned int mhtonl(unsigned int value);
unsigned int mntohl(unsigned int value);

unsigned int mstrtoul(const char *str);

unsigned short crc16_compute(unsigned char *buf, unsigned short bytes);
unsigned short checksum16(unsigned char *buf, unsigned short size);
unsigned int checksum32(unsigned char *buf, unsigned int size);

void show_hexdump(void *buf, unsigned int offset, int count);

// #ifdef __cplusplus
// }
// #endif


#endif /* __UTILS_H */
