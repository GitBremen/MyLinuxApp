#ifndef __SERIAL_H
#define __SERIAL_H

#include <strings.h>
#include <stdio.h>
#include <termios.h>

int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop);
int serial_fd;
#endif