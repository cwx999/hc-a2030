#include "cgpio.h"
#include "iodef.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


cgpio::cgpio()
{
    // gpio_init();
}

cgpio::~cgpio()
{
}

int cgpio::gpio_init()
{
    int i = 0, ret = 0;
    int flags = 0;
    unsigned int value = 0;

    fd = open(GPIO_DEV, O_RDWR);
    if (fd < 0) {
        printf("open %s err:\n", GPIO_DEV);
        perror("");
        return -1;
    }

    ret = fcntl(fd, F_SETOWN, getpid());
    if (ret < 0) {
        perror("fcntl() F_SETOWN");
    }
    flags = fcntl(fd, F_GETFD);
    if (flags < 0) {
        perror("fcntl() F_GETFD");
    }

    ret = fcntl(fd, F_SETFL, flags | FASYNC);
    if (ret < 0) {
        perror("fcntl() F_SETFL");
    }

    return 0;
}

int cgpio::gpio_uninit()
{
    close(fd);
    return 0;
}

int cgpio::gpio_set_value(unsigned int pin, unsigned int value)
{
    pthread_mutex_lock(&gMutex);
    int fd = 0, ret = 0;
    gpio_ctrl_t gpio = {
        .pin = pin, 
        .value = value
    };

    fd = open(GPIO_DEV, O_RDWR);
    if (fd < 0) {
        printf("open %s err:\n", GPIO_DEV);
        perror("");
        return -1;
    }

    ret = write(fd, &gpio, sizeof(gpio_ctrl_t));
    if (ret < 0) {
        printf("set gpio pin <%d> err:\n", pin);
        perror("");
    }

    close(fd);
    pthread_mutex_unlock(&gMutex);
    return ret;
}

int cgpio::gpio_get_value(unsigned int pin, unsigned int *value)
{
    pthread_mutex_lock(&gMutex);
    int fd = 0, ret = 0;
    gpio_ctrl_t gpio = {
        .pin = pin, 
        .value = 0
    };

    fd = open(GPIO_DEV, O_RDWR);
    if (fd < 0) {
        printf("open %s err:\n", GPIO_DEV);
        perror("");
        return -1;
    }

    ret = read(fd, &gpio, sizeof(gpio_ctrl_t));
    if (ret < 0) {
        printf("read gpio pin <%d> err:\n", pin);
        perror("");
    }
    *value = gpio.value;
    close(fd);
    pthread_mutex_unlock(&gMutex);
    return ret;
}

uint16_t cgpio::get_insert_stat(void)
{
    uint32_t stat = 0;
    uint16_t online = 0;
    gpio_get_value(BD_INS_10, &stat);
    online |= ((~stat) & 0x01) << 9;
    gpio_get_value(BD_INS_9, &stat);
    online |= ((~stat) & 0x01) << 8;
    gpio_get_value(BD_INS_8, &stat);
    online |= ((~stat) & 0x01) << 7;
    gpio_get_value(BD_INS_7, &stat);
    online |= ((~stat) & 0x01) << 6;
    gpio_get_value(BD_INS_6, &stat);
    online |= ((~stat) & 0x01) << 5;
    gpio_get_value(BD_INS_5, &stat);
    online |= ((~stat) & 0x01) << 4;
    gpio_get_value(BD_INS_4, &stat);
    online |= ((~stat) & 0x01) << 3;
    gpio_get_value(BD_INS_3, &stat);
    online |= ((~stat) & 0x01) << 2;
    gpio_get_value(BD_INS_2, &stat);
    online |= ((~stat) & 0x01) << 1;
    gpio_get_value(BD_INS_1, &stat);
    online |= ((~stat) & 0x01);
    // printf("status:%x\n", online);
    return online;
}






