#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdint.h>


class CNetwork
{
private:
    int file_size(const char *file);
    int file_read(const char *file, void *buf, uint32_t size);

public:
    CNetwork() {}
    ~CNetwork() {}

    int set_inet_addr(const char *dev, const char *addr);
    int get_inet_addr(const char *dev, char *addr);

    int set_inet_mask(const char *dev, const char *mask);
    int get_inet_mask(const char *dev, char *mask);

    int add_gateway(const char *dev, const char *gateway);
    int del_gateway(const char *dev, const char *gateway);
    int get_gateway(const char *dev, char *gateway);

    int set_nameserver(const char *dns1, const char *dns2);
    int get_nameserver(char *dns1, char *dns2);

    int interface_dhcp(const char *dev);
    int interface_static(const char *dev, 
                        const char *inet_addr, 
                        const char *inet_mask, 
                        const char *gateway);
    
    uint32_t ipstr_to_u32(const char *ip);
    int ipstr_to_u8buf(uint8_t *buf, const char *ip);
};


#endif
