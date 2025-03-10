#include "warehouse.h"
#include "serial.h"
#include "sqlite.h"
#include <sys/epoll.h>

extern void *pthread_uart(void *args);
extern void *pthread_process(void *args);
extern void *pthread_sqlite(void *args);
extern void *pthread_MQTT(void *args);

char uartbufR[1024];
char uartbufW[1024];

int main(int argc, char *argv[])
{
    pthread_t uart_id;
    pthread_t process_id;
    pthread_t sqlite_id;
    pthread_t mqtt_id;

    /****************程序初始化部分****************/
    warehouse_Init();                            // 初始化仓库数据参数
    sqlite_Init();                               // 初始化数据库
    if (MQTTCLIENT_SUCCESS != MQTTClient_Init()) // 初始化MQTT客户端
    {
        printf("MQTT client init failed\n");
        return -1;
    }
    serial_fd = open(UART3_DEV, O_RDWR, 0777);
    if (serial_fd < 0)
    {
        printf("open uart3 failed\n");
        return -1;
    }
    printf("open uart3 success\n");
    set_opt(serial_fd, 115200, 8, 'N', 1);

    memset(uartbufR, '\0', sizeof(uartbufR));
    memset(uartbufW, '\0', sizeof(uartbufW));

    /*创建epoll实例*/
    int epfd = epoll_create(1);
    if (epfd < 0)
    {
        perror("epoll_create");
        exit(1);
    }

    /*添加串口文件描述符到epoll实例*/
    struct epoll_event ev;
    ev.data.fd = serial_fd;
    ev.events = 0;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serial_fd, &ev);

    pthread_create(&uart_id, NULL, pthread_uart, NULL);
    pthread_detach(uart_id);

    printf("main function enter while...\n");
    while (1)
    {
        if (epoll_flag)
        {
            ev.data.fd = serial_fd;
            ev.events = EPOLLIN;
            epoll_ctl(epfd, EPOLL_CTL_MOD, serial_fd, &ev);

            epoll_flag = 0;
        }
        else
        {
            ev.data.fd = serial_fd;
            ev.events = 0;
            epoll_ctl(epfd, EPOLL_CTL_MOD, serial_fd, &ev);
        }
        while (epoll_wait(epfd, &ev, 1, 500) < 0)
        {
            printf("enter epoll_wait error while\n");
            if (errno == EINTR)
            {
                continue;
            }
            perror("epoll_wait");
            exit(1);
        }
        if (ev.data.fd == serial_fd && ev.events & EPOLLIN)
        {
            pthread_create(&sqlite_id, NULL, pthread_sqlite, NULL);
            pthread_detach(sqlite_id);
            pthread_create(&process_id, NULL, pthread_process, warehouse);
            pthread_join(process_id, NULL);

            pthread_create(&mqtt_id, NULL, pthread_MQTT, NULL);
            pthread_detach(mqtt_id);
        }
    }
    free(warehouse);
    return 0;
}
