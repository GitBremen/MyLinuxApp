#ifndef __CAMERA_H
#define __CAMERA_H

#define CAMERA_DEV "/dev/video0"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

struct plane_start
{
    void *start;
};

struct buffer
{
    struct plane_start *plane_start;
    struct v4l2_plane *planes_buffer;
};

#endif
