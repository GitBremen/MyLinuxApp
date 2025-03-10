#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

#define MAX_BUF_SIZE 1024
#define IN_FILES 3
#define TIME_DELAY 10000
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(int argc, char *argv[])
{
    struct pollfd fds[IN_FILES];
    char buf[MAX_BUF_SIZE];
    int i, ret, real_read, maxfd;

    /*按照一定权限打开两个源文件*/
    fds[0].fd = 0;
    // fds[1].fd = open("file2", O_RDONLY);

    if (fds[1].fd = open("file1", O_RDONLY | O_NONBLOCK) < 0)
    {
        printf("open file1 error\n");
        return -1;
    }
    if (fds[2].fd = open("file2", O_RDONLY | O_NONBLOCK) < 0)
    {
        printf("open file2 error\n");
        return -1;
    }

    /*设置pollfd结构体中的events成员*/
    for (i = 0; i < IN_FILES; i++)
    {
        fds[i].events = POLLIN;
    }

    /*循环测试是否存在正在监听的文件描述符*/
    while (fds[0].events || fds[1].events || fds[2].events)
    {
        if (poll(fds, IN_FILES, -1) < 0)
        {
            printf("poll error or timeout\n");
            return -1;
        }

        for (i = 0; i < IN_FILES; i++)
        {
            /*判断在哪个文件上发生了事件*/
            if (fds[i].revents)
            {
                memset(buf, 0, MAX_BUF_SIZE);
                real_read = read(fds[i].fd, buf, MAX_BUF_SIZE);
                if (real_read < 0)
                {
                    /*系统错误，结束运行*/
                    printf("read error\n");
                    return -1;
                }
                else if (real_read == 0)
                {
                    /*取消对该文件的监听*/
                    printf("file %d was closed\n", i + 1);
                    fds[i].events = 0;
                    close(fds[i].fd);
                }
                else
                {
                    /*在标准输入上有数据时*/
                    if (i == 0)
                    {
                        if ((buf[0] == 'q') || (buf[0] == 'Q'))
                        {
                            return -1;
                        }
                        /*将读取的数据显示到终端上*/
                        else
                        {
                            buf[real_read] = '\0';
                            printf("read from stdin:%s\n", buf);
                        }
                    }
                    // printf("read from file %d:%s\n", i + 1, buf);
                }
            }
        }
    }
    exit(0);
}