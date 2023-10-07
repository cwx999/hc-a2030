#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <net/if.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string>
#include "network.h"


int CNetwork::set_inet_addr(const char *dev, 
                            const char *addr)
{
    int fd = 0;
    int ret = 0;

    struct ifreq ifr;
    struct sockaddr_in *sin;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket error! ");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, dev);

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;

    ret = inet_aton(addr, &(sin->sin_addr));
    if (ret < 0)
    {
        perror("inet_aton error! ");
        goto err_exit;
    }

    ret = ioctl(fd, SIOCSIFADDR, &ifr);
    if (ret < 0)
    {
        perror("ioctl SIOCSIFADDR error! ");
        goto err_exit;
    }

    close(fd);

    return 0;

err_exit:
    close(fd);

    return -1;
}

int CNetwork::get_inet_addr(const char *dev, 
                            char *addr)
{
    int fd = 0;
    int ret = 0;

    struct ifreq ifr;
    struct sockaddr_in *sin;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket error! ");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, dev);

    ret = ioctl(fd, SIOCGIFADDR, &ifr);
    if (ret < 0)
    {
        perror("ioctl SIOCGIFADDR error! ");
        goto err_exit;
    }

    sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    strcpy(addr, inet_ntoa(sin->sin_addr));

    close(fd);

    return 0;

err_exit:
    close(fd);

    return -1;
}

int CNetwork::set_inet_mask(const char *dev, 
                            const char *mask)
{
    int fd = 0;
    int ret = 0;

    struct ifreq ifr;
    struct sockaddr_in *sin;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket error! ");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, dev);

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;

    ret = inet_aton(mask, &(sin->sin_addr));
    if (ret < 0)
    {
        perror("inet_aton error! ");
        goto err_exit;
    }

    ret = ioctl(fd, SIOCSIFNETMASK, &ifr);
    if (ret < 0)
    {
        perror("ioctl SIOCSIFNETMASK error! ");
        goto err_exit;
    }

    close(fd);

    return 0;

err_exit:
    close(fd);

    return -1;
}

int CNetwork::get_inet_mask(const char *dev, 
                            char *mask)
{
    int fd = 0;
    int ret = 0;

    struct ifreq ifr;
    struct sockaddr_in *sin;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket error! ");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, dev);

    ret = ioctl(fd, SIOCGIFNETMASK, &ifr);
    if (ret < 0)
    {
        perror("ioctl SIOCGIFNETMASK error! ");
        goto err_exit;
    }

    sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    strcpy(mask, inet_ntoa(sin->sin_addr));

    close(fd);

    return 0;

err_exit:
    close(fd);

    return -1;
}

int CNetwork::add_gateway(const char *dev, 
                            const char *gateway)
{
    int fd = 0;
    int ret = 0;

    struct ifreq ifr;
    struct sockaddr_in *sin;
    struct rtentry rt;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket error! ");
        return -1;
    } 

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, dev);

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;

    memset(&rt, 0, sizeof(struct rtentry));
    memset(sin, 0, sizeof(struct sockaddr_in));

    sin->sin_family = AF_INET;
    sin->sin_port = 0;

    ret = inet_aton(gateway, &sin->sin_addr);
    if (ret < 0)
    {
        perror("inet_aton error! ");
        goto err_exit;
    }

    memcpy(&rt.rt_gateway, sin, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family = AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family = AF_INET;
    rt.rt_flags = RTF_GATEWAY;

    ret = ioctl(fd, SIOCADDRT, &rt);
    if (ret < 0)
    {
        perror("ioctl SIOCADDRT error! ");
        goto err_exit;
    }

    close(fd);

    return 0;

err_exit:
    close(fd);

    return -1;
}

int CNetwork::del_gateway(const char *dev, 
                            const char *gateway)
{
    int fd = 0;
    int ret = 0;

    struct ifreq ifr;
    struct sockaddr_in *sin;
    struct rtentry rt;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket error! ");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, dev);

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;

    memset(&rt, 0, sizeof(struct rtentry));
    memset(sin, 0, sizeof(struct sockaddr_in));

    sin->sin_family = AF_INET;
    sin->sin_port = 0;

    ret = inet_aton(gateway, &sin->sin_addr);
    if (ret < 0)
    {
        perror("inet_aton error! ");
        goto err_exit;
    }

    memcpy(&rt.rt_gateway, sin, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family = AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family = AF_INET;
    rt.rt_flags = RTF_GATEWAY;

    ret = ioctl(fd, SIOCDELRT, &rt);
    if (ret < 0)
    {
        perror("ioctl SIOCDELRT error! ");
        goto err_exit;
    }

    close(fd);

    return 0;

err_exit:
    close(fd);

    return -1;
}

int CNetwork::get_gateway(const char *dev, 
                            char *gateway)
{
    int i = 0;
    int ret = 0;
    FILE *pp = NULL;
    char *ptr = NULL;
    char msg[1024] = {0};

    pp = popen("ip route", "r");
    if (NULL == pp) {
        perror("popen ip route error! ");
        return -1;
    }

    ret = fread(msg, 1, sizeof(msg), pp);
    if (ret < 0) {
        pclose(pp);
        return -1;
    }

    pclose(pp);

    for (i = 0; i < sizeof(msg); i++) {
        ptr = strstr(msg + i, "default via");
        if (NULL != ptr) {
            sscanf(ptr, "default via %s dev", gateway);
            break;
        }
    }

    if (NULL == ptr) {
        // printf("%s\n", msg);
        return -1;
    }
    
    return 0;
}
int CNetwork::set_nameserver(const char *dns1, 
                            const char *dns2)
{
    int ret = 0;
    FILE *fp = NULL;
    char buf1[32] = {0};
    char buf2[32] = {0};

    sprintf(buf1, "nameserver %s\n", dns1);
    sprintf(buf2, "nameserver %s\n", dns2);

    fp = fopen("/etc/resolv.conf", "wb");
    if (NULL == fp)
    {
        perror("fopen resolv.conf error");
        return -1;
    }

    ret = fwrite(buf1, strlen(buf1), 1, fp);
    if (ret < 0)
    {
        printf("fwrite %s to resolv.conf error\n", buf1);
        perror("");
    }

    ret = fwrite(buf2, strlen(buf2), 1, fp);
    if (ret < 0)
    {
        printf("fwrite %s to resolv.conf error\n", buf2);
        perror("");
    }

    fclose(fp);

    return 0;
}

int CNetwork::get_nameserver(char *dns1, 
                            char *dns2)
{
    int ret = 0;
    char tmp[256] = {0};
    char *buf = NULL;
    char *ptr = NULL;
    int cnt = 0;
    int size = 0;

    ret = access("/etc/resolv.conf", F_OK);
    if (ret < 0) {
        printf("No such file or directory\n");
        return -1;
    }

    size = file_size("/etc/resolv.conf");

    buf = (char *)malloc(size);
    if (NULL == buf) {
        perror("malloc ");
        return -1;
    }

    file_read("/etc/resolv.conf", buf, size);

    ptr = buf;

    while (NULL != ptr) {

        ptr = strstr(ptr, "nameserver");
        if (NULL == ptr) {
            break;
        }

        if ((ptr[11] < '0') || (ptr[11] > '9')) {
            ptr++;
            continue;
        }

        cnt++;

        memset(tmp, 0, sizeof(tmp));
        sscanf(ptr, "nameserver %s", tmp);
        // printf("%d, %s\n", cnt, tmp);

        if (1 == cnt) {
            memcpy(dns1, tmp, strlen(tmp));
        }

        if (cnt >= 2) {
            memcpy(dns2, tmp, strlen(tmp));
            break;
        }
        ptr++;
    }

    free(buf);

    return cnt;
}

int CNetwork::interface_dhcp(const char *dev)
{
    int i = 0;
    int ret = 0;
    FILE *fp = NULL;
    char line[10][256] = {0};

    sprintf(line[0], "# /etc/network/interfaces -- "
                        "configuration file for ifup(8), ifdown(8)\n");
    sprintf(line[1], "\n");
    sprintf(line[2], "# The loopback interface\n");
    sprintf(line[3], "auto lo\n");
    sprintf(line[4], "iface lo inet loopback\n");
    sprintf(line[5], "\n");
    sprintf(line[6], "# Wired or wireless interfaces\n");
    sprintf(line[7], "auto %s\n", dev);
    sprintf(line[8], "iface %s inet dhcp\n", dev);
    sprintf(line[9], "\n");

    fp = fopen("/etc/network/interfaces", "wb");
    if (NULL == fp) {
        perror("fopen /etc/network/interfaces err:");
        return -1;
    }

    for (i = 0; i < 10; i++) {
        ret = fwrite(line[i], strlen(line[i]), 1, fp);
        if (ret < 0) {
            perror("fwrite interfaces err:");
        }
    }

    fclose(fp);
    
    return 0;
}

int CNetwork::interface_static(const char *dev, 
                        const char *inet_addr, 
                        const char *inet_mask, 
                        const char *gateway)
{
    int i = 0;
    int ret = 0;
    FILE *fp = NULL;
    char line[14][256] = {0};
    std::string str_mac;
    std::string str_tmp = inet_addr;
    size_t count = str_tmp.find_last_of(".");
    if (count == std::string::npos)
    {
        printf("string error\n");
        return -1;
    }
    str_mac = str_tmp.substr(count+1, str_tmp.length() - count);

    sprintf(line[0],  "# /etc/network/interfaces -- "
                        "configuration file for ifup(8), ifdown(8)\n");
    sprintf(line[1],  "\n");
    sprintf(line[2],  "# The loopback interface\n");
    sprintf(line[3],  "auto lo\n");
    sprintf(line[4],  "iface lo inet loopback\n");
    sprintf(line[5],  "\n");
    sprintf(line[6],  "# Wired or wireless interfaces\n");
    sprintf(line[7],  "auto %s\n", dev);
    sprintf(line[8],  "iface %s inet static\n", dev);
    sprintf(line[9],  "address %s\n", inet_addr);
    sprintf(line[10], "netmask %s\n", inet_mask);
    sprintf(line[11], "gateway %s\n", gateway);
    sprintf(line[12], " hwaddress enter 00:0A:35:00:02:%02x\n",   atoi(str_mac.c_str()));
    sprintf(line[13], "\n");

    fp = fopen("/etc/network/interfaces", "wb");
    if (NULL == fp) {
        perror("fopen /etc/network/interfaces err:");
        return -1;
    }

    for (i = 0; i < 14; i++) {
        ret = fwrite(line[i], strlen(line[i]), 1, fp);
        if (ret < 0) {
            perror("fwrite interfaces err:");
        }
    }
    fclose(fp);
    
    return 0;
}

int CNetwork::ipstr_to_u8buf(uint8_t *buf, const char *ip)
{
    int ret = 0;

    if ((NULL == ip) || 
        ((NULL == buf))) {
        return -1;
    }

    ret = sscanf(ip, "%d.%d.%d.%d", 
                buf, buf + 1, buf + 2, buf + 3);
    if (4 != ret) {
        memset(buf, 0, 4);
    }

    return ret;
}

uint32_t CNetwork::ipstr_to_u32(const char *ip)
{
    int ret = 0;
    uint8_t buf[4] = {0};
    uint32_t result = 0;

    if (NULL == ip) {
        return 0;
    }

    ret = sscanf(ip, "%d.%d.%d.%d", 
                buf, buf + 1, buf + 2, buf + 3);
    if (4 != ret) {
        printf("match %d, %d-%d-%d-%d\n", ret, 
                buf[0], buf[1], buf[2], buf[3]);
        return 0;
    }

    result = (buf[0] << 24) | (buf[1] << 16) | 
            (buf[2] << 8) | buf[3];
    return result;
}

int CNetwork::file_size(const char *file)
{
    int ret = 0;
    int size = 0;
    struct stat stat_buff;

    ret = stat(file, &stat_buff);
    if (ret < 0)
    {
        printf("stat %s error\n", file);
        perror("");
        return -1;
    }

    size = stat_buff.st_size;

    return size;
}

int CNetwork::file_read(const char *file, void *buf, uint32_t size)
{
    int ret = 0;
    FILE *fp = NULL;

    fp = fopen(file, "rb");
    if (NULL == fp)
    {
        printf("fopen %s error\n", file);
        perror("");
        return -1;
    }

    ret = fread(buf, size, 1, fp);
    if (ret < 0)
    {
        printf("fread %s error\n", file);
        perror("");
    }

    fclose(fp);

    return ret;
}
