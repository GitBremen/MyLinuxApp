#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "database.h"
#include "stdShared.h"
#include <pthread.h>


/* 处理注册的业务逻辑 */
int handleRegisterLoigc(int connfd, const char * name, const char * passwd)
{
    ResponseMsg resMsg;
    /* 业务类型 */
    resMsg.bussinessType = REGISTER;

    /* 1. 该用户是否已经存在 */
    if (userIsExist(name) == 1)
    {
        /* 注册失败 */
        /* 告诉客户端 */
        resMsg.code = REGISTER_ERROR;
    }
    else
    {
        /* 将用户名和密码存储到数据库中 */
        resMsg.code = REGISTER_SUCCESS;
        saveUserInfo(name, passwd);
    }
    /* 回复客户端  */
    write(connfd, &resMsg, sizeof(resMsg));

    return 0;
}

/* 处理登陆的业务逻辑 */
int handleLoginLoigc(int connfd, const char * name, const char * passwd)
{
    ResponseMsg resMsg;
    /* 业务类型 */
    resMsg.bussinessType = LOGIN;

    /* 1. 该用户是否已经存在 */
    if (userIsExist(name) == 0)
    {
        /* 登陆失败 - 没有该用户 */
        /* 告诉客户端 */
        resMsg.code = LOGIN_NOUSER_ERROR;
    }
    else
    {
        /* 该用户已经存在 */
        /* 判断用户名和密码是否匹配 */
        if (userMatchPasswd(name, passwd) == 0)
        {
            /* 用户名和密码不匹配 */
            resMsg.code = LOGIN_NOMATCH_ERROR;
        }
        else
        {
            /* 用户名和密码匹配 */
            resMsg.code = LOGIN_SUCCESS;

            /* 维护用户名和通信句柄的映射 <-> */
            saveUserMapConnfd(name, connfd);
        }
    }
    /* 回复客户端  */
    write(connfd, &resMsg, sizeof(resMsg));
    
    return 0;
}

/* 添加好友 */
void handleAddFriendLoigc(int connfd, const char * fromName, const char * toName)
{
    ResponseMsg resMsg;
    resMsg.bussinessType = ADDFRIEND;
    /* 1. 判断toName用户是否存在 */
    if (userIsExist(toName) == 0)
    {
        /* 用户不存在 */
        resMsg.code = ADDFRIEND_NOUSER_ERROR;
    }
    else 
    {
        /* 用户是存在的 */
        /* 将添加好友的信息发送给toName */
        int sidefd = accordCUserNameGetConnfd(toName);
        printf("toName = %s, sidefd = %d\n", toName, sidefd);

        /* 将信息发送给toName */
        /* 接收端 */
        resMsg.code = ADDFRIEND_RECEIVE_APPLY;
        strcpy(resMsg.fromName, fromName);
        write (sidefd, &resMsg, sizeof(resMsg));
    }
    /* 回复发送端客户端 */
    resMsg.code = ADDFRIEND_REQUEST_SUCCESS;
    write(connfd, &resMsg, sizeof(resMsg));
}

/* 线程处理函数 */
void * thread_func(void *arg)
{
    int connfd = *(int *)arg;
    char buffer[1024] = { 0 };
    while (1)
    {
        /* 接收客户端的通信数据 */
        int readBytes = read(connfd, buffer, sizeof(buffer));
        if (readBytes < 0)
        {
            printf("readBytes < 0\n");
            break;
        }
        else if (readBytes == 0)
        {
            printf("readBytes == 0\n");
            break;
        }
        else if (readBytes > 0)
        {
            Message * msg = (Message *)(buffer);
            
            if (msg->bussinessType == REGISTER)     /* 注册 */
            {
                handleRegisterLoigc(connfd, msg->name, msg->passwd);
                printf("msg->bussinessType = %d, msg->name = %s, msg->passwd = %s\n", msg->bussinessType, msg->name, msg->passwd);
            }
            else if (msg->bussinessType == LOGIN)   /* 登陆 */
            {
                handleLoginLoigc(connfd, msg->name, msg->passwd);
                printf("msg->bussinessType = %d, msg->name = %s, msg->passwd = %s\n", msg->bussinessType, msg->name, msg->passwd);
            }
            else if (msg->bussinessType == ADDFRIEND)   /* 添加好友 */
            {
                handleAddFriendLoigc(connfd, msg->fromname, msg->toname);
                printf("msg->bussinessType = %d, msg->fromname = %s, msg->toname = %s\n", msg->bussinessType, msg->fromname, msg->toname);
            }
        }
    }

    /* 用户下线处理 */
    delUserMapConnfd(connfd);

    /* 退出线程 */
    pthread_exit(NULL);
}

int main()
{
    /* 文件描述符 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket error\n");
        exit(-1);
    }
    printf("sockfd = %d\n", sockfd);

    // 设置端口复用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* 绑定ip和端口 */
    struct sockaddr_in localAddress;
    memset(&localAddress, 0, sizeof(localAddress));
    /* 地址族 */
    localAddress.sin_family = AF_INET;
    /* IP */
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    /* 端口 */
    localAddress.sin_port = htons(8080);
    int ret = bind(sockfd, (const struct sockaddr *)&localAddress, sizeof(localAddress));
    if (ret != 0)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听 */
    ret = listen(sockfd, 10);
    if (ret != 0)
    {
        printf("listen error");
        exit(-1);
    }

    /* 等待客户端连接 */
    printf("等待客户端连接...\n");
    while (1)
    {
        int connfd = accept(sockfd, NULL, NULL);
        printf("connfd = %d\n", connfd);

        pthread_t tid;
        pthread_create(&tid, NULL, thread_func, (void *)&connfd);

        /* 休眠1S. */
        sleep(1);
    }

    return 0;
}