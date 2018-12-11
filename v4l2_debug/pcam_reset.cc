/*
Target board: ZYBO Z7-20
Kernel&Circuit:https://github.com/Digilent/Petalinux-Zybo-Z7-20
Distribution: Ubuntu 16.04
https://rcn-ee.com/rootfs/eewiki/minfs/ubuntu-16.04.3-minimal-armhf-2017-10-07.tar.xz

Thanks to @hokim.
Xilinx Form: https://forums.xilinx.com/t5/Embedded-Linux/can-t-get-image-from-PCam-5C-on-Ubuntu-running-on-ZYBO-Z7-20/m-p/882879#M28057

Before run this code, you must run the following commands.
sudo media-ctl -d /dev/media0 -V '"ov5640 2-003c":0 [fmt:UYVY/'640x480'@1/'60' field:none]'
sudo media-ctl -d /dev/media0 -V '"43c60000.mipi_csi2_rx_subsystem":0 [fmt:UYVY/'640x480' field:none]'
*/
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include <linux/videodev2.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#define FMT_NUM_PLANES 3
#define WIDTH 640
#define HEIGHT 480

using namespace std;
struct buffer_addr_struct{
  void *start[FMT_NUM_PLANES];
  size_t length[FMT_NUM_PLANES];
} *buffers;

static int xioctl(int fd, int request, void *arg){
	int r;
	do {
		r = ioctl (fd, request, arg);
		if (request == VIDIOC_DQBUF) {
			std::cout << "r : " << r << std::endl;
		}
	} while (-1 == r && EINTR == errno);
	return r;
}

int main() {

	unsigned char *buffer;

	// 1. Open Video Device.
	int fd;
	fd = open("/dev/video0", O_RDWR, 0);
	if (fd == -1){
	    std::cout << "Failed to open video device." << std::endl;
	    return 1;
	}
	
	struct 	v4l2_buffer buf;
	memset(&(buf), 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	// 10. Turn off streaming.
	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &buf.type)) {
		std::cout << "VIDIOC_STREAMOFF" << std::endl;
	}

	close(fd);

	return 0;
}	
