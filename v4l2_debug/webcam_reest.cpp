#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <chrono>

#include <linux/videodev2.h>

using namespace std;
#define REQUEST_BUFFER_NUM 10

static int xioctl(int fd, int request, void *arg)
{
    int r;
	do {
		r = ioctl (fd, request, arg);
	} while (-1 == r && EINTR == errno);
	return r;
}
unsigned char *buffers[4];

int main() {

	// 1. Open Video Device.
	int fd;
	fd = open("/dev/video1", O_RDWR, 0);
	if (fd == -1)
	{
	    std::cout << "Failed to open video device." << std::endl;
	    return 1;
	}
	// 10. Turn off streaming.
	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &buf.type)) {
		std::cout << "VIDIOC_STREAMOFF" << std::endl;
	}

	return 0;
}