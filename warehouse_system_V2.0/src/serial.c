#include "serial.h"
int serial_fd = 0;
/*串口通用参数设置函数*/
int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio; // 定义结构体newtio和oldtio
    // 将原串口的数据取到oldtio
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    // 将newio清零和设置c_cflag
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD; // 使能接收和忽略控制线
    newtio.c_cflag &= ~CSIZE;
    // 设置数据位
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    // 设置校验位
    switch (nEvent)
    {
    // 偶校验
    case 'O':
        newtio.c_cflag |= PARENB;           // 使能奇偶校验
        newtio.c_cflag |= PARODD;           // 偶校验
        newtio.c_iflag |= (INPCK | ISTRIP); // 输入校验并忽略第八位
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD; // 取消偶校验（置零偶校验位），开启奇校验
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB; // 不进行奇偶校验
        break;
    }
    // 设置波特率
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    // 设置停止位
    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB; // 一位停止位
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB; // 两位停止位

    newtio.c_cc[VTIME] = 10; // 不设置读取超时
    newtio.c_cc[VMIN] = 0;   // 读取最小字符数为0

    tcflush(fd, TCIFLUSH); // 清空缓冲区

    // 使配置生效
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    //	printf("set done!\n\r");
    return 0;
}
