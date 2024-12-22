#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PORT 8088
#define backlog_num 10

int main(int argc, char *argv[])
{
    int iSocketServer;
    int iSocketClient;
    int iRet;
    int RecvLen;
    int iClientNum = -1;

    unsigned char ucRecvBuf[1024];

    struct sockaddr_in tSocketServerAddr;
    struct sockaddr_in tSocketClientAddr;

    iSocketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == iSocketServer)
    {
        printf("socket error!\n");
        return -1;
    }
    tSocketServerAddr.sin_family = AF_INET;
    tSocketServerAddr.sin_port = htons(SERVER_PORT);
    tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
    memset(tSocketServerAddr.sin_zero, 0, 8);

    iRet = bind(iSocketServer, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
    if (-1 == iRet)
    {
        printf("bind error!\n");
        return -1;
    }

    iRet = listen(iSocketServer, backlog_num); // backlog最大可以监听几路连接
    if (-1 == iRet)
    {
        printf("listen error!\n");
        return -1;
    }

    while (1)
    {
        int addrlen = sizeof(struct sockaddr);
        iSocketClient = accept(iSocketServer, (struct sockaddr *)&tSocketClientAddr, &addrlen);
        if (-1 != iSocketClient)
        {
            iClientNum++;
            // inet_ntoa将tSocketClientAddr.sin_addr转换为ASCII字符串
            printf("Get connect from %s\n", inet_ntoa(tSocketClientAddr.sin_addr));

            if (!fork())
            {
                // 子进程的源码
                while (1)
                {
                    // 接收客户端发来的数据并显示出来
                    RecvLen = recv(iSocketClient, ucRecvBuf, strlen(ucRecvBuf), 0);
                    if (RecvLen <= 0)
                    {
                        close(iSocketClient);
                        return -1;
                    }
                    else
                    {
                        ucRecvBuf[RecvLen] = '\0';
                        printf("Get message from %d : %s", iClientNum, ucRecvBuf);
                    }
                }
            }
        }
    }
    close(iSocketServer);
    return 0;
}
