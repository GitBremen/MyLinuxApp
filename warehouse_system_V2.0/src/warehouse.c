#include "warehouse.h"

extern char uartbufR[1024];
extern char uartbufW[1024];

struct storage_info *warehouse = NULL;
int epoll_flag = 0;
/*初始化互斥锁*/
pthread_mutex_t uart_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t warehouse_data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t warehouse_status_mutex = PTHREAD_MUTEX_INITIALIZER;

/*初始化条件变量*/
pthread_cond_t warehouse_status_cond = PTHREAD_COND_INITIALIZER;

void warehouse_Init(void)
{
    warehouse = (struct storage_info *)malloc(sizeof(struct storage_info));
    memset(warehouse, 0, sizeof(struct storage_info));
    warehouse->temperature_max = 30;
    warehouse->temperature_min = 10;
    warehouse->illumination_max = 1000;
    warehouse->illumination_min = 500;
}

/*接收环境信息线程*/
void *pthread_uart(void *args)
{
    int numPrase = 0;
    int nByte = 0;
    while (1)
    {
        pthread_mutex_lock(&uart_mutex);
        if ((nByte = read(serial_fd, uartbufR, sizeof(uartbufR))))
        {
            pthread_mutex_lock(&warehouse_data_mutex);
            char *p = NULL;
            p = strstr(uartbufR, "t:");
            numPrase = sscanf(p, "t:%f,i:%d",
                              &(warehouse->temperature), &(warehouse->illumination));
            pthread_mutex_unlock(&warehouse_data_mutex);
            if (numPrase == 2)
            {
                printf("temperature:%f,illumination:%d\n",
                       warehouse->temperature, warehouse->illumination);
            }
            write(serial_fd, p, nByte);
            epoll_flag = 1;
        }
        pthread_mutex_unlock(&uart_mutex);
        nByte = 0;
    }
    pthread_exit(NULL);
}

/*MQTT线程*/
void *pthread_MQTT(void *args)
{
    printf("MQTT thread start running...\n");
    char dbuf[32] = {0};
    pthread_mutex_lock(&warehouse_status_mutex);
    strcpy(dbuf, uartbufW);
    pthread_mutex_unlock(&warehouse_status_mutex);
    if (MQTTCLIENT_SUCCESS != MQTTClientPublish(WAREHOUSE_TOPIC, dbuf))
    {
        fprintf(stderr, "publish message error");
    }
    printf("publish message:%s\n", dbuf);

    pthread_exit(NULL);
}

/*处理环境信息线程*/
void *pthread_process(void *args)
{
    pthread_mutex_lock(&warehouse_status_mutex);
    if (warehouse->temperature > warehouse->temperature_max ||
        warehouse->temperature < warehouse->temperature_min)
    {
        warehouse->buzzer_status = 1;
    }
    else
    {
        warehouse->buzzer_status = 0;
    }

    if (warehouse->illumination < warehouse->illumination_min)
    {
        warehouse->led_status = 1;
    }
    else
    {
        warehouse->led_status = 0;
    }

    memset(uartbufW, '\0', sizeof(uartbufW));
    sprintf(uartbufW, "led:%d,buzzer:%d",
            warehouse->led_status, warehouse->buzzer_status);
    pthread_mutex_unlock(&warehouse_status_mutex);

    write(serial_fd, uartbufW, strlen(uartbufW));
    pthread_exit(NULL);
}
