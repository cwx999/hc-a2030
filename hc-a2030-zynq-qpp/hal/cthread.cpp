#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include "cthread.h"
#include "cgpio.h"
#include "iodef.h"
#include "cnetcfg.h"
#include "../components/utils/utils.h"
#include <sys/reboot.h>
#include "../components/protocol/protcan.h"

int cmd_idle_flag = 0;
int reboot_flag = 0;;
CHProtCan hpcan;
cgpio m_io;
unsigned char *all_slot_recv_buf;
wake_up_ wake_up_t;
uint8_t slot_device_online[10] = {0};
uint8_t slot_device_online_count[10][5] = {{0}};
struct slot_vol_cur m_slot_all_volcur;
void* t_vol_cur_req(void* arg);
void* t_run_led(void* arg);
void* t_dected_net_rst(void* arg);
void* t_wake_up(void* arg);

void cthread_init(void)
{
    pthread_t vol_cur_req_run;
    pthread_t run_led_run;
    pthread_t net_detected_reset_run;
    pthread_t wake_up_run;
    if ((pthread_create(&vol_cur_req_run, NULL, t_vol_cur_req, NULL)) == -1)
    {
        printf("create vol_cur_req_run thread error!\n");
    }
    printf("create vol_cur_req_run thread success\n");

    if ((pthread_create(&run_led_run, NULL, t_run_led, NULL)) == -1)
    {
        printf("create run_led_run thread error!\n");
    }
    printf("create run_led_run thread success\n");

    if ((pthread_create(&net_detected_reset_run, NULL, t_dected_net_rst, NULL)) == -1)
    {
        printf("create net_detected_reset_run thread error!\n");
    }
    printf("create net_detected_reset_run thread success\n");
    if ((pthread_create(&wake_up_run, NULL, t_wake_up, NULL)) == -1)
    {
        printf("create wake_up_run thread error!\n");
    }
    printf("create wake_up_run thread success\n");
}

void* t_vol_cur_req(void* arg)
{
    int ret = 0;
    uint16_t iostat = 0;
    uint16_t ioStat_old;
    //如果空闲，接收每个槽位的电压电流数据
    all_slot_recv_buf = (unsigned char*)malloc(200);
    can_stream_t m = {0}, s = {0};
    unsigned char slot_recv_buf[20];
    m.bytes = 20;
    uint16_t vol, cur;
    int index = 0;
    m.buf = slot_recv_buf;
    
    while(1)
    {
        if (CMD_BUSY != cmd_idle_flag)
        {

            //获取槽在位状态
            index = 0;
            cmd_idle_flag = CMD_BUSY;
            if (reboot_flag  ==1)
            {
                sleep(7);
                reboot_flag = 0;
            }
            memset(&m_slot_all_volcur, 0,sizeof(struct slot_vol_cur));
            iostat = m_io.get_insert_stat();
            for (int i = 0; i < 10; i++)
            {
                if (((iostat >> i) & 0x01) == 1 )
                {
                    
                    m.id.value = hpcan.genExtId(0x210,i+1, false, true,0);
                    s.buf = slot_recv_buf;
                    ret = hpcan.forward(&m, &s);
                    if (ret != 0)
                    {
                        usleep(200000);
                        continue;
                    }
                    memcpy(all_slot_recv_buf+i*20,slot_recv_buf, 20);
                    
                    memset(slot_recv_buf,0, 20);
                }
                usleep(10000);
                
            }
            memcpy(&m_slot_all_volcur, all_slot_recv_buf, 200);
            //检测每个槽位5个通道，如果其中一个通道电流值连续3次大于0，判断为有待测设备，唤醒标志置1，
            //超3次没有，唤醒标志置零
            slot_device_online_check();
            //  printf("line:%d\n",__LINE__);
            cmd_idle_flag = CMD_IDLE;
           
            usleep(1000000);
        }else
        {
            usleep(200000);
        }
    }
    
}

void* t_run_led(void* arg)
{
    cgpio m_io;
    while (1)
    {
        m_io.gpio_set_value(WDG_WDI, HIGH);
        usleep(500000);
        m_io.gpio_set_value(WDG_WDI, LOW);
        usleep(500000);
    }
    
}

void* t_dected_net_rst(void* arg)
{
    cgpio m_io;
    uint32_t io_stat = 0;
    while(1)
    {
        m_io.gpio_get_value(RST_KEY, &io_stat);
        if (HIGH != io_stat)
        {
            sleep(3);
            m_io.gpio_get_value(RST_KEY, &io_stat);
            if (HIGH != io_stat)
            {
                printf("net reset\n");
                set_net_default();
                usleep(1000000);
                sync();
                reboot(RB_AUTOBOOT);
            }
        }
        usleep(50000);

    }
}

void* t_wake_up(void* arg)
{
    int ret = 0;
    uint16_t iostat = 0;
    int step = 0;
    uint32_t can_id = 0;
    CHPortCanFd hpcanfd;
    CHProtCan mhpcan[10];
    struct can_frame i_frame;
    can_pack_t can_pkt;
    can_pkt.frames = 1;
    can_stream_t m_stm_t;
    std::string strDev = "can";
    for (int i = 1; i<= 10; i++)
    {
        std::string strDev = "can";
        strDev.append(std::to_string(i));
        mhpcan[i -1].init(strDev);
    }

    while (1)
    {
        if (wake_up_t.start)
        {
           
            if (step == 0)//查询一次在位状态
            {
                iostat = m_io.get_insert_stat();

                step = 1;
            }
            can_id = u8buf_to_u32(wake_up_t.data);
            // printf("id:0x%x\n", can_id);
            // printf("data: %02x %02x %02x %02x %02x %02x %02x %02x\n",
            //             wake_up_t.data[4],wake_up_t.data[5],wake_up_t.data[6],wake_up_t.data[7],
            //             wake_up_t.data[8],wake_up_t.data[9],wake_up_t.data[10],wake_up_t.data[11]);
            if (can_id == 0)//如果未下发指令，使用默认值
            {
                u32_to_u8buf(wake_up_t.data, 0x500);
            }
            can_id = u8buf_to_u32(wake_up_t.data);
            m_stm_t.id.value = can_id;

            m_stm_t.bytes = 8;
            m_stm_t.buf = wake_up_t.data+4;
            
            for (int i = 0; i < 10; i++)//需考虑槽在位状态
            {
                if (((iostat >> i) & 0x01) == 1)
                {
                    if (slot_device_online[i] == 0) continue;
                    // printf("slot[%d]send start\n",i+1);
                    mhpcan[i].packet(&can_pkt, &m_stm_t);
                    ret = mhpcan[i].transmit(&can_pkt);
                    //ret = hpcanfd.transmit(i, (void*)wake_up_t.data,sizeof(wake_up_t.data));
                    // printf("slot[%d]send end,ret %d,count %d\n",i+1,ret, count);
                    // usleep(50000);
                }
            }
            usleep(500000);
            // printf("line%d\n",__LINE__);
        }
        else
        {
            step = 0;
            usleep(2000000);
        }
    }
    printf("thread exit\n");
}

void slot_device_online_check()
{
    for (int slot = 0; slot < 10; slot++) {//10个槽
        for (int chl = 0; chl < 5; chl++) {//5个通道
            // printf("slot:chl:vol:cur[%d:%d:%d:%d]\n",slot+1, chl+1, 
            // m_slot_all_volcur.vc_slot[slot].vc_chl[chl].vol,
            // m_slot_all_volcur.vc_slot[slot].vc_chl[chl].cur);
            if (m_slot_all_volcur.vc_slot[slot].vc_chl[chl].cur > 0) {
                if ((slot_device_online_count[slot][chl]++) >6)
                    slot_device_online_count[slot][chl] = 6;
            }else {
                if(( slot_device_online_count[slot][chl]--) == 0)
                slot_device_online_count[slot][chl] = 0;
            }
        }
    }
    //5个通道任意一个次数大于5次，该槽位可以唤醒
    for(int slot = 0; slot < 10; slot++) {
        for(int chl = 0; chl < 5; chl++) {
            if (slot_device_online_count[slot][chl] > 20) {
                slot_device_online[slot] = 1;
                // printf("slot:chl:count:[%d:%d:%d]\n",slot+1, chl+1, slot_device_online_count[slot][chl]);
                break;
            }
            slot_device_online[slot] = 0;
        }
    }
}