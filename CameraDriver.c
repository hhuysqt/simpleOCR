/* 
 * camera 'dirver'
 *
 */

#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "CameraDriver.h"


/* 
 * varibles
 */
const int var_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
const char VideoDeviceName[] = "/dev/video0";
int video_fd; // video device handler
struct v4l2_buffer st_desbuf;
struct v4l2_capability		st_camera_cap;
struct v4l2_fmtdesc			st_support_fmt;
struct v4l2_format			st_camera_fmt;

// mmap address and size
void *camera_buffer = NULL;
int camera_buffer_size = 0;


/* 
 * function definition
 */

/* 
 * private function
 * LowLevelInit
 *	print out camera info
 * input: pixelformat, can be V4L2_PIX_FMT_MJPEG or V4L2_PIX_FMT_YUYV
 */
static void CameraLowLevelInit(CameraInitFmt pixelformat)
{
	/* Open the camera */
	video_fd = open(VideoDeviceName, O_RDWR);
	if(video_fd < 0){
		fprintf(stderr, "\nFailed to open %s, error msg: %s\n\n", VideoDeviceName, strerror(errno));
		exit(-1);
	} else {
		printf("\nOpened %s.\n\n",VideoDeviceName);
	}


	/* list its information */
	ioctl(video_fd, VIDIOC_QUERYCAP, &st_camera_cap);
	printf("Driver Name:\n\t%s\nCard Name:\n\t%s\nBus Info:\n\t%s\nDriver Version:\n\t%u.%u.%u\n\n",
				st_camera_cap.driver,
				st_camera_cap.card,
				st_camera_cap.bus_info,
				0xff & (st_camera_cap.version >> 16),
				0xff & (st_camera_cap.version >> 8 ),
				0xff &  st_camera_cap.version
		  );

	/* list its supported format */
	st_support_fmt.index = 0;
	st_support_fmt.type = var_type;
	printf("Supported format:\n");
	while(-1 != ioctl(video_fd, VIDIOC_ENUM_FMT, &st_support_fmt))
	{
		printf("\t%d: %s\n", 
					++st_support_fmt.index, 
					st_support_fmt.description
			  );
	}


	/* set output pixel format */
	st_camera_fmt.type = var_type;
	st_camera_fmt.fmt.pix.pixelformat = ((pixelformat == CameraInitFmtMJPEG) ? V4L2_PIX_FMT_MJPEG : V4L2_PIX_FMT_YUYV);
	st_camera_fmt.fmt.pix.height = 32;
	st_camera_fmt.fmt.pix.width = 32;
	st_camera_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	if(-1 == ioctl(video_fd, VIDIOC_S_FMT, &st_camera_fmt)){
		fprintf(stderr, "\nFailed to set MJPEG, error msg: %s\n\n", strerror(errno));
		exit(-2);
	} else {
		printf("\nSet to MJPEG!\n\n");
	}


	/* list current range */
	ioctl(video_fd, VIDIOC_G_FMT, &st_camera_fmt);
	printf("%dX%d\n\n",
				st_camera_fmt.fmt.pix.width,
				st_camera_fmt.fmt.pix.height
		  );
}


/* 
 * interfaces
 * CameraInit
 *	initialize the camera:
 *	- call LowLevelInit
 *	- memory mapping
 * input: pixelformat, can be CameraInitFmtMJPEG or CameraInitFmtYUYV
 */
int CameraInit(CameraInitFmt pixelformat)
{
	struct v4l2_requestbuffers	st_reqbuf;

	CameraLowLevelInit(pixelformat);

	/*
	 * memory mapping!!!
	 */
	/* set buffered capture */
	st_reqbuf.count = 1;
	st_reqbuf.type = var_type;
	st_reqbuf.memory = V4L2_MEMORY_MMAP;
	ioctl(video_fd, VIDIOC_REQBUFS, &st_reqbuf);

	memset(&st_desbuf, 0, sizeof(st_desbuf));

	/* get buffer information, e.g. size, start, etc */
	st_desbuf.type = var_type;
	st_desbuf.memory = V4L2_MEMORY_MMAP;
	st_desbuf.index = 0;
	if(-1 == ioctl(video_fd, VIDIOC_QUERYBUF, &st_desbuf)){
		fprintf(stderr, "Failed to locate image buffer, error msg: %s\n\n", strerror(errno));
		return(-3);
	}

	camera_buffer_size = st_desbuf.length;
	camera_buffer = mmap(
				NULL,
				st_desbuf.length,
				PROT_WRITE | PROT_READ,
				MAP_SHARED,
				video_fd,
				st_desbuf.m.offset
				);
	if(MAP_FAILED == camera_buffer){
		fprintf(stderr, "mmap() failed, error msg: %s\n\n", strerror(errno));
		return(-4);
	} else {
		printf("mmap() OK.\n\tstart:%X       length:%d\n\n",
					(unsigned int)camera_buffer,
					camera_buffer_size
			  );
	}

	/* query buffers */
	ioctl(video_fd, VIDIOC_QBUF, &st_desbuf);
	/* open data stream... */
	ioctl(video_fd, VIDIOC_STREAMON, &var_type);

	return 0;
}


/* 
 * CameraShut
 *	take a picture
 */
void CameraShoot(void)
{
	ioctl(video_fd, VIDIOC_DQBUF, &st_desbuf);
#ifdef DEBUGGING
	printf("++++++++++++\n");
#endif
	ioctl(video_fd, VIDIOC_QBUF,  &st_desbuf);
}

/*
 * CameraGetPicture
 * 	input: 	pointer to the address pointer
 * 	return:	picture size
 */
int CameraGetPicture(void**addr)
{
	*addr = camera_buffer;
	return camera_buffer_size;
}

int CameraGetWidth(void)
{
	return st_camera_fmt.fmt.pix.width;
}
int CameraGetHeight(void)
{
	return st_camera_fmt.fmt.pix.height;
}

