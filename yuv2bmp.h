#ifndef _MY_YUV2BMP_H_
#define _MY_YUV2BMP_H_

#ifdef __cplusplus
extern "C" {
#endif

int YUV422ToBMP(int outfilefd, unsigned char* inbuf, int width, int height);
void YUV422ToRGB(char *outbuff, char *inbuff, int size);

#ifdef __cplusplus
}
#endif

#endif
