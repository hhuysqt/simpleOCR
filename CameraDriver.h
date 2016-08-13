#ifndef _MY_CAMERA_DRIVER_H_
#define _MY_CAMERA_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	CameraInitFmtMJPEG,
	CameraInitFmtYUYV
}CameraInitFmt;

// Initialize the camera using v4l2
int CameraInit(CameraInitFmt pixelformat);
// Take a photo
void CameraShoot(void);
// Get the photo's start address and size
int CameraGetPicture(void**addr);
int CameraGetWidth(void);
int CameraGetHeight(void);

#ifdef __cplusplus
}
#endif

#endif

