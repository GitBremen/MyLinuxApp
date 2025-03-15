#ifndef __WAERHOUSE_H
#define __WAERHOUSE_H

#include "goods.h"
#include "serial.h"
#include "mqtt.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define LED_DEV "/dev/led"
#define BUZZER_DEV "/dev/buzzer"
#define UART3_DEV "/dev/ttyS3"

#define GOODS_NUM 10
struct storage_info
{
    unsigned char storage_status;
    unsigned char led_status;
    unsigned char fan_status;
    unsigned char buzzer_status;
    char samplingTime[20];
    float temperature;
    float temperature_max;
    float temperature_min;
    int illumination;
    int illumination_max;
    int illumination_min;
    struct storage_goods_info goods_info[GOODS_NUM];
};

struct storage_info *warehouse;
int epoll_flag;

void warehouse_Init(void);
#endif
