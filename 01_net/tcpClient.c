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
    int iSocketClient;
    int iRet;
    int SendLen;
    struct sockaddr_in tSocketClientAddr;
    struct sockaddr_in tSocketServerAddr;

    unsigned char ucSendBuf[1024];

    if (argc != 2)
    {
        printf("Usage:%s <serverIP>\n", argv[0]);
    }

    iSocketClient = socket(AF_INET, SOCK_STREAM, 0);

    tSocketServerAddr.sin_family = AF_INET;
    tSocketServerAddr.sin_port = htons(SERVER_PORT);
    // tSocketClientAddr.sin_addr.s_addr = INADDR_ANY;
    if (0 == inet_aton(argv[1], &tSocketServerAddr.sin_addr))
    {
        printf("invalid serverIP\n");
        return -1;
    }
    memset(tSocketServerAddr.sin_zero, 0, 8);

    iRet = connect(iSocketClient, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
    if (-1 == iRet)
    {
        printf("connect error\n");
        return -1;
    }

    while (1)
    {
        if (fgets(ucSendBuf, 1023, stdin))
        {
            SendLen = send(iSocketClient, ucSendBuf, strlen(ucSendBuf), 0);
            if (SendLen <= 0)
            {
                close(iSocketClient);
                return -1;
            }
        }
    }

    return 0;
}