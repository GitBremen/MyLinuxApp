#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "stdShared.h"
#include "menuInterface.h"

/* 全局变量 - 通信句柄 */
int g_sockfd;

/* 注册 */
void registerInfo(int g_sockfd)
{
    Message msg;

    char name[128] = { 0 };
    printf("请输入你的用户名:");
    scanf("%s", name);

    char passwd[128] = { 0 };
    printf("请输入你的密码:");
    scanf("%s", passwd);

    msg.bussinessType = REGISTER;
    strcpy(msg.name, name);
    strcpy(msg.passwd, passwd);

    /* 把msg的信息发送到服务器 */
    write(g_sockfd, (const void *)&msg, sizeof(msg));
}

/* 登陆 */
const char * loginInfo(int g_sockfd)
{
    Message msg;

    char name[128] = { 0 };
    printf("请输入你的用户名:");
    scanf("%s", name);

    char passwd[128] = { 0 };
    printf("请输入你的密码:");
    scanf("%s", passwd);

    msg.bussinessType = LOGIN;
    strcpy(msg.name, name);
    strcpy(msg.passwd, passwd);

    /* 把msg的信息发送到服务器 */
    write(g_sockfd, (const void *)&msg, sizeof(msg));

    char * tmpName = (char *)malloc(sizeof(char) * 128);
    strcpy(tmpName, name);
    
    return tmpName;
}

int main()
{
    /* 创建文件描述符 */
    g_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_sockfd == -1)
    {
        printf("socket error\n");
        exit(-1);
    }
    printf("g_sockfd = %d\n", g_sockfd);


    struct sockaddr_in serverAddr;
    /* 服务器的地址族  */
    serverAddr.sin_family = AF_INET;
    /* 服务器的端口 */
    serverAddr.sin_port = htons(8080);
    /* 服务器的IP */
    inet_pton(AF_INET, "172.25.23.103", (void *)&(serverAddr.sin_addr.s_addr));

    /* 连接服务器 */
    int ret = connect(g_sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret != 0)
    {
        perror("connect error");
        exit(-1);
    }

    /* 程序走到这个地方, 连接服务器成功... */
    printf("连接服务器成功...\n");

    int choice = 0;
    while (1)
    {
        /* 清屏 */
        system("clear");

        /* 显示菜单功能 */
        menuInterface();

        printf("请输入你的选择:");
        scanf("%d", &choice);

        const char * loginName = NULL;
        switch (choice)
        {
        case 1:
            /* 注册 */
            registerInfo(g_sockfd);
            break; 
        case 2:
            /* 登陆 */
            loginName = loginInfo(g_sockfd);
            break;
        default:
            break;
        }

        char buffer[1024] = { 0 };
        /* 接收服务器的信息 */
        read(g_sockfd, buffer, sizeof(buffer));
        
        ResponseMsg *resMsg = (ResponseMsg *)buffer;
        if (resMsg->bussinessType == REGISTER)
        {
            if (resMsg->code == REGISTER_ERROR)
            {
                printf("注册失败\n");
            }
            else if (resMsg->code == REGISTER_SUCCESS)
            {
                printf("注册成功\n");
            }
        }
        else if (resMsg->bussinessType == LOGIN)
        {
            if (resMsg->code == LOGIN_NOUSER_ERROR)
            {
                printf("不存在此用户, 请先注册\n");
            }
            else if (resMsg->code == LOGIN_NOMATCH_ERROR)
            {
                printf("密码错误, 请重新输入\n");
            }
            else if (resMsg->code == LOGIN_SUCCESS)
            {
                printf("登陆成功\n");
                sleep(2);
                /* 登陆成功之后 */
                afterLoginSuccess(loginName);
            }
        }
        /* 休眠2S */
        sleep(2);
    }
    
    



    return 0;
}