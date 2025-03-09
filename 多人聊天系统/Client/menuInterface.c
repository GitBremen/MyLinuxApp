#include "menuInterface.h"
#include <stdio.h>
#include "stdShared.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/* 引入全局变量 */
extern int g_sockfd;

void menuInterface()
{
    printf("1. 注册\n");
    printf("2. 登陆\n");
}

/* 业务菜单 */
void bussinessMenu()
{
    printf("1. 添加好友\n");
    printf("2. 删除好友\n");
    printf("3. 私人聊天\n");
}

/* 添加好友 */
void addFriend(const char * fromName)
{
    Message msg;
    msg.bussinessType = ADDFRIEND;

    char toname[128] = { 0 };
    printf("请输入添加的用户名:");
    scanf("%s", toname);

    /* 打包 */
    strcpy(msg.toname, toname);
    strcpy(msg.fromname, fromName);

    /* 发信息给服务器 */
    write(g_sockfd, (void *)&msg, sizeof(msg));

    

}

/* 删除好友 */
void delFriend()
{

}

/* 私人聊天 */
void privateChat()
{

}

/* 线程函数 */
void * thread_func(void * arg)
{
    /* 接收服务器的回复信息 */
    char buffer[1024] = { 0 };
    read(g_sockfd, buffer, sizeof(buffer));

    ResponseMsg *resMsg = (ResponseMsg *)buffer;

    if (resMsg->code == ADDFRIEND_NOUSER_ERROR)
    {
        printf("添加失败:没有该用户\n");
    }
    else if (resMsg->code == ADDFRIEND_REQUEST_SUCCESS)
    {
        printf("添加好友请求发送成功\n");
    }
    else if (resMsg->code == ADDFRIEND_RECEIVE_APPLY)
    {
        printf("收到一条来自%s好友申请\n", resMsg->fromName);
    }
    
    pthread_exit(NULL);
}

/* 登陆成功之后 */
void afterLoginSuccess(const char * loginName)
{
    /* 清屏 */
    system("clear");

    /* 开辟线程 */
    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);
    while (1)
    {    
        /* 显示业务菜单 */
        bussinessMenu();

        int choice = 0;
        printf("请选择功能:");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            /* 添加好友 */
            addFriend(loginName);
            break;
        case 2:
            /* 删除好友 */
            delFriend();
            break;
        case 3:
            /* 私人聊天 */
            privateChat();
            break;
        default:
            break;
        }
    }
}