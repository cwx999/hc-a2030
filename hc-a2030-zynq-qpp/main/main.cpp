#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../components/utils/utils.h"
#include "../hal/halprot.h"

#include "../components/socketcan/socketcan.h"
#include "../hal/cthread.h"


int main(int argc, const char **argv)
{
#if 0
    int ret = 0;
    CSocketCan canfd;
    struct canfd_frame fdframe;

    if (argc != 2) {
        printf("need device\n");
        return -1;
    }

    ret = canfd.openDev(argv[1], true);
    if (ret < 0) {
        perror("open can3:");
        return -1;
    }

    while (1) {
        ret = canfd.receive(&fdframe, sizeof(fdframe));
        if (ret <= 0) {
            perror("receive");
            break;
        }
        // printf("fdframe.flags: %d\n", fdframe.flags);

        ret = canfd.transmit(&fdframe, sizeof(fdframe));
        if (ret <= 0) {
            perror("transmit");
            break;
        }
    }

    canfd.closeDev();
    return 0;
#endif

    int ret = 0;
    printf("\n");
    printf(" * Multi-channel voltage and current acquisition unit\n");
    printf(" * HC-A2030-1 Verion %08x build in %08x\n", FW_ARM_VERSION, FW_ARM_DATE);
    printf(" * 2022 - 20xx Copyright by zlzt team\n");

    ret = hpudp.init();
    ret = hpcan.init();
    cthread_init();
    while (1) {
        halProtocolHandle(func_list);
        usleep(100);
    }
    
    return 0;
}
