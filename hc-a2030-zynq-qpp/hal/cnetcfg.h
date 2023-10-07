#ifndef __CNETCFG_H__
#define __CNETCFG_H__
#include <string>

#define NET_NAME    "eth0"
#define def_port 8080

void set_net_default(void);
int setNet(std::string ip,std::string mask, std::string gateway);
int setPort(std::string port);
int getPort(void);



bool ipv4Check(std::string IP);
#endif

