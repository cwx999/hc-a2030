#ifndef __CGPIO_H__
#define __CGPIO_H__
#include <pthread.h>
#include <stdint.h>
#include <sys/ioctl.h>
#define GPIO_DEV "/dev/zynq_gpio"

#define IOC_MAGIC 'M'
#define IOC_RD_GPIO  _IOR(IOC_MAGIC, 0, int)
#define IOC_WR_GPIO  _IOW(IOC_MAGIC, 1, int)

#define SW_OFT  9  

#define HIGH 1
#define LOW 0

typedef struct {
    unsigned int pin;
    unsigned int value;
}gpio_ctrl_t;

class cgpio
{

public:
    cgpio();
    ~cgpio();
    int gpio_init();
    int gpio_uninit();
    
    int gpio_set_value(unsigned int pin, unsigned int value);
    int gpio_get_value(unsigned int pin, unsigned int *value);
    //fun
    uint16_t get_insert_stat(void);
private:
    int fd;
    pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
};

#endif

