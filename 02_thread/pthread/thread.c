#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct info
{
    int age;
    char name[20];
    float score;
};
struct info info_test = {0};

void *thread1(void *args)
{
    while (1)
    {
        struct info *info_test = (struct info *)args;
        pthread_mutex_lock(&mutex); // 互斥锁上锁
        info_test->age = 20;
        strcpy(info_test->name, "reira");
        info_test->score = 99.9;
        printf("thread1 age:%d, name:%s, score:%f\n",
               info_test->age, info_test->name, info_test->score);
        pthread_mutex_unlock(&mutex); // 互斥锁解锁
        sleep(1);
    }

    pthread_exit(NULL);
}

void *thread2(void *args)
{
    while (1)
    {
        struct info *info_test = (struct info *)args;
        pthread_mutex_lock(&mutex); // 互斥锁上锁
        info_test->age = 30;
        strcpy(info_test->name, "aira");
        info_test->score = 98.9;
        printf("thread2 age:%d, name:%s, score:%f\n",
               info_test->age, info_test->name, info_test->score);
        pthread_mutex_unlock(&mutex); // 互斥锁解锁
        sleep(1);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_create(&tid1, NULL, thread1, &info_test);
    pthread_create(&tid2, NULL, thread2, &info_test);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    // pthread_detach(tid1);
    // pthread_detach(tid2);
    while (1)
    {
        sleep(1);
        pthread_mutex_lock(&mutex); // 互斥锁上锁
        printf("main thread\n");

        pthread_mutex_unlock(&mutex); // 互斥锁解锁
    }
    return 0;
}