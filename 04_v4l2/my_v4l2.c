#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <linux/fb.h>

#define devicename "/dev/video0"

int main(int argv, char *argc[])
{
    int ret;
    int i;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_format vfmt;
    struct v4l2_requestbuffers reqbuf;
    struct v4l2_buffer vbuf;
    unsigned char *mptr[4];
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    struct v4l2_buffer rbuf;

    /*1.打开设备*/
    int v4l2_fd = open(devicename, O_RDWR);
    if (v4l2_fd < 0)
    {
        perror("open error");
        return -1;
    }

    /*2.获取摄像头支持的格式*/
    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    // ret = ioctl(v4l2_fd, VIDIOC_ENUM_FMT, &fmtdesc);
    while (ioctl(v4l2_fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0)
    {
        // 将枚举出来的格式以及描述信息存放在数组中
        // cam_fmts[fmtdesc.index].pixelformat = fmtdesc.pixelformat;
        // strcpy(cam_fmts[fmtdesc.index].description, fmtdesc.description);
        unsigned char *p = (unsigned char *)&fmtdesc.pixelformat;
        printf("index = %d,pixelformat = %c%c%c%c\n", fmtdesc.index, p[0], p[1], p[2], p[3]);
        printf("index = %d,description = %s\n", fmtdesc.index, fmtdesc.description);
        printf("\n");
        fmtdesc.index++;
    }

    /*3.设置格式*/
    memset(&vfmt, 0, sizeof(vfmt));
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    vfmt.fmt.pix.width = 640;
    vfmt.fmt.pix.height = 480;
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
    if ((ret = ioctl(v4l2_fd, VIDIOC_S_FMT, &vfmt)) < 0)
    {
        perror("VIDIOC_S_FMT error");
        return -1;
    }

    /*4.获取格式*/
    memset(&vfmt, 0, sizeof(vfmt));
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if ((ret = ioctl(v4l2_fd, VIDIOC_G_FMT, &vfmt)) < 0)
    {
        perror("VIDIOC_G_FMT error");
        return -1;
    }

    /*5.申请内核缓冲区队列*/
    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbuf.count = 4;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    if ((ret = ioctl(v4l2_fd, VIDIOC_REQBUFS, &reqbuf)) < 0)
    {
        perror("VIDIOC_REQBUFS error");
        return -1;
    }

    /*6.将内核的缓冲队列映射到用户空间*/
    // memset(&vbuf, 0, sizeof(vbuf));
    // vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    // vbuf.memory = V4L2_MEMORY_MMAP;
    // vbuf.m.planes = (struct v4l2_plane *)calloc(4, sizeof(struct v4l2_plane));
    for (i = 0; i < reqbuf.count; ++i)
    {
        memset(&vbuf, 0, sizeof(vbuf));
        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        vbuf.memory = V4L2_MEMORY_MMAP;
        vbuf.length = 2;
        vbuf.m.planes = (struct v4l2_plane *)calloc(vbuf.length, sizeof(struct v4l2_plane));

        vbuf.index = i;
        if ((ret = ioctl(v4l2_fd, VIDIOC_QUERYBUF, &vbuf)) < 0)
        {
            perror("VIDIOC_QUERYBUF error");
            return -1;
        }
        // printf("vbuf.m.planes[0].length = %d\n", vbuf.m.planes[0].length);
        // printf("vbuf.m.planes[0].m.mem_offset = %d\n", vbuf.m.planes[0].m.mem_offset);
        mptr[i] = (unsigned char *)mmap(NULL, vbuf.length,
                                        PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, vbuf.m.offset);

        if ((ret = ioctl(v4l2_fd, VIDIOC_QBUF, &vbuf)) < 0)
        {
            perror("VIDIOC_QBUF error");
            return -1;
        }
    }

    /*7.开始采集*/
    if ((ret = ioctl(v4l2_fd, VIDIOC_STREAMON, &type)) < 0)
    {
        perror("采集失败");
        return -1;
    }

    // 从队列中取出一帧数据
    memset(&rbuf, 0, sizeof(rbuf));
    rbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    rbuf.memory = V4L2_MEMORY_MMAP;
    rbuf.length = 2;
    rbuf.m.planes = (struct v4l2_plane *)calloc(rbuf.length, sizeof(struct v4l2_plane));
    if ((ret = ioctl(v4l2_fd, VIDIOC_DQBUF, &rbuf)) < 0)
    {
        perror("从队列中取出一帧数据失败");
        return -1;
    }

    printf("Plane 0 length: %d, Plane 1 length: %d\n", rbuf.m.planes[0].length, rbuf.m.planes[1].length);
    FILE *file = fopen("my.yuv", "w+");
    fwrite(mptr[rbuf.index], 1, rbuf.m.planes[0].length, file);
    fwrite(mptr[rbuf.index] + rbuf.m.planes[0].length, 1, rbuf.m.planes[1].length, file);

    fflush(file);
    fclose(file);

    // 通知内核已经使用完毕
    if ((ret = ioctl(v4l2_fd, VIDIOC_QBUF, &rbuf)) < 0)
    {
        perror("VIDIOC_QBUF error");
        return -1;
    }

    /*8.停止采集*/
    if ((ret = ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type)) < 0)
    {
        perror("VIDIOC_STREAMOFF error");
        return -1;
    }

    close(v4l2_fd);
    return 0;
}
