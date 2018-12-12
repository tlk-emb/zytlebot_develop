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

	//unsigned char *buffer;

	// 1. Open Video Device.
	int fd;
	fd = open("/dev/video0", O_RDWR, 0);
	if (fd == -1){
	    std::cout << "Failed to open video device." << std::endl;
	    return 1;
	}

	// 2. Querying video capabilities.
	struct v4l2_capability caps;
	memset(&caps, 0, sizeof(caps));
	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps)){
		std::cout << "Failed to query capabilities." << std::endl;
	    return 1;
	}
	std::cout << "bus_info	: " << caps.bus_info << std::endl;
	std::cout << "card		: " << caps.card << std::endl;
	std::cout << "driver	: " << caps.driver << std::endl;
	std::cout << "version	: " << caps.version << std::endl;

	// 3. Format Specification.
	{
		struct v4l2_format fmt;
		memset(&(fmt), 0, sizeof(fmt));

		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		fmt.fmt.pix_mp.width = WIDTH;
		fmt.fmt.pix_mp.height = HEIGHT;
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUYV;
		fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;
		
		if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
			std::cout << "Failed to set pixel format." << std::endl;
			return 1;
		}
	}

	int num_planes;
	struct v4l2_requestbuffers reqbuf;
	const int MAX_BUF_COUNT = 3;/*we want at least 3 buffers*/
	
	// 4. Request Buffer
	{
		memset(&(reqbuf), 0, sizeof(reqbuf));
		reqbuf.count = FMT_NUM_PLANES;
		reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		reqbuf.memory = V4L2_MEMORY_MMAP;

		if (-1 == xioctl(fd, VIDIOC_REQBUFS, &reqbuf)){
			std::cout << "Failed to request buffer." << std::endl;
			return 1;
		}
		if (reqbuf.count < MAX_BUF_COUNT){
			std::cout << "Not enought buffer memory." << std::endl;
			return 1;
		}
		std::cout << "reqbuf.count : " << reqbuf.count << std::endl;

		buffers = (buffer_addr_struct*) calloc(reqbuf.count, sizeof(*buffers));
		// assert(buffers != NULL);
				  
	}

	// 5. Query Buffer
	{
		for(int i = 0; i < reqbuf.count; i++){ 

			struct  v4l2_plane planes[FMT_NUM_PLANES];
			struct  v4l2_buffer buf;
			memset(&(buf), 0, sizeof(buf));
			memset(planes, 0, sizeof(planes));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.m.planes = planes;
			buf.length = FMT_NUM_PLANES;
			buf.index = i;
			if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)){
				std::cout << "Failed to query buffer." << std::endl;
				return 1;
			}
			num_planes = buf.length;
			std::cout << "buf.length : " << buf.length << std::endl;
			std::cout << "buf.m.offset : " << buf.m.offset << std::endl;

			for(int j = 0; j < num_planes; j++){
				buffers[i].length[j] = buf.m.planes[j].length;
				std::cout << "buf.m.planes[j].length : " << buf.m.planes[j].length << std::endl;
				buffers[i].start[j] = mmap(NULL, buf.m.planes[j].length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.planes[j].m.mem_offset);
				if(MAP_FAILED == buffers[i].start[j]){
					std::cout << "mmap error" << std::endl;
				}
				std::cout << "buffers[i].start[j] : " << buffers[i].start[j] << std::endl; 
			
			}
		}
	}

	//5.5
	cout << "now 5.5 " << endl;
	for (int i = 0; i < reqbuf.count; ++i) {
        struct v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        struct v4l2_plane planes[FMT_NUM_PLANES];
        buf.m.planes = planes;
        buf.length = FMT_NUM_PLANES;
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            std::cerr << "VIDIOC_QBUF" << std::endl;
    }

	// 6. Start Streaming
	{
		struct 	v4l2_buffer buf;
		memset(&(buf), 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		if(-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type))
		{
			std::cout << "Fail to start Capture" << std::endl;
			return 1;
		}
	}
	char camdata[WIDTH*HEIGHT*2];
	
	struct 	v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;
	struct v4l2_plane planes[FMT_NUM_PLANES];
	buf.m.planes = planes;
	buf.length = FMT_NUM_PLANES;

	//You can get image repeatedly by this loop
	for (int i = 0; i < 100; i++) {
	// 7. Capture Image
	

			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			struct timeval tv = {0};
			tv.tv_sec = 2;
			int r = select(fd+1, &fds, NULL, NULL, &tv);

			if(-1 == r){
				std::cout << "Waiting for Frame" << std::endl;
				return 1;
			}
	
			if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf)){
				std::cout << "Retrieving Frame" << std::endl;
				return 1;
			}

			// Connect buffer to queue for next capture.
			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
				std::cout << "VIDIOC_QBUF" << std::endl;
			}
		

		// 8. Store Image in Array
		{
		  for(int j = 0; j < num_planes; j++){
		    memcpy(camdata, buffers[0].start[j], WIDTH*HEIGHT*2);
		  }
		}

		// 9. Save YUYV raw Image
		// You can convert raw image to png by this code.
		// https://gist.github.com/lp6m/209ab987e2bc25261d8b1c6ef1f3777d
		{
		  FILE* fp = fopen("./camdata.dat", "wb");
		  fwrite(camdata, sizeof(camdata), 1, fp);
		  fclose(fp);
		}
	

	}
	
	// 10. Turn off streaming.
	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &buf.type)) {
		std::cout << "VIDIOC_STREAMOFF" << std::endl;
	}

	for(int i = 0; i < 3; i++){
        if(munmap(buffers[i].start[0], 480 * 640 * 2) != 0){
            cerr << "pcam munmap failed" << endl;
        }else{
            cout << "pcam munmap success" << endl;
        }
    }

	close(fd);

	return 0;
}	