#include "cnetcfg.h"
#include "network.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
CNetwork m_net;

void set_net_default(void)
{
    std::string s_ip = "192.168.0.10";
    std::string s_mask = "255.255.255.0";
    std::string s_gateway = "192.168.0.1";
    std::string s_port = "8080";

    m_net.interface_static(NET_NAME, 
                            s_ip.c_str(),
                            s_mask.c_str(),
                            s_gateway.c_str());
    setPort(s_port);
    
}

int setNet(std::string ip,std::string mask, std::string gateway)
{
    int ret = 0;
    if(!ipv4Check(ip)) return -1;
    if(!ipv4Check(mask)) return -2;
    if(!ipv4Check(gateway)) return -3;

    ret = m_net.interface_static(NET_NAME, ip.c_str(), mask.c_str(), gateway.c_str());
    return ret;
}

int setPort(std::string port)
{
    std::ofstream ofs;
    int i_port = atoi(port.c_str());
    ofs.open("/etc/a2030_port.cfg", std::ios::out | std::ios::trunc);
    ofs.write((const char*)&i_port, sizeof(int));
    ofs.close();
    return 0;
}

int getPort(void)
{
    char pp[4] = {0};
    std::string port;
    std::ifstream ifs;
    uint32_t u_port;
    int cur_port;
    
    ifs.open("/etc/a2030_port.cfg", std::ios::in);
    if (!ifs.is_open())
    {
        // cout<<"error to open port.cfg,port is default value!"<<endl;
        return def_port;
    }

	ifs.getline(pp, 4);
    ifs.close();
    memcpy(&u_port, pp, 4);
    
    if (u_port != 0)
    {
        cur_port = u_port;
    }else
    {
        cur_port = def_port;
    }
    return cur_port;
    
}




bool ipv4Check(std::string IP)
{
    int dotcnt = 0;
    //数一共有几个.
    for (int i = 0; i < IP.length(); i++)
    {
        if (IP[i] == '.')
            dotcnt++;
    }
    //ipv4地址一定有3个点
    if (dotcnt != 3)
        return false;
    std::string temp = "";
    for (int i = 0; i < IP.length(); i++)
    {
        if (IP[i] != '.')
            temp += IP[i];
        //被.分割的每部分一定是数字0-255的数字
        if (IP[i] == '.' || i == IP.length() - 1)
        {
            if (temp.length() == 0 || temp.length() > 3)
                return false;
            for (int j = 0; j < temp.length(); j++)
            {
                if (!isdigit(temp[j]))
                    return false;
            }
            int tempInt = atoi(temp.c_str());
            if (tempInt > 255 || tempInt < 0)
                return false;
            // std::string convertString = std::to_string(tempInt);
            // if (convertString != temp)
            //     return false;
            temp = "";
        }
    }
    if (IP[IP.length()-1] == '.')
        return false;
    return true;

}

