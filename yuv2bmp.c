#include "yuv2bmp.h"
#include "bmp.h"
#include <unistd.h>
#include <string.h>


inline char _Clamp(int data)
{
	return data > 255 ? 255 : data < 0 ? 0 : data;
}


int YUV422ToBMP(int outfilefd, unsigned char* inbuf, int width, int height)
{
	BITMAPFILEHEADER bmph;
	BITMAPINFOHEADER bmpi;
	int i, j;
	BMPFillHeader(&bmph, &bmpi, width, height);
	write(outfilefd, &bmph, sizeof(BITMAPFILEHEADER));
	write(outfilefd, &bmpi, sizeof(BITMAPINFOHEADER));
	// YUV422: 2 bytes per pixel
	inbuf += width*height*2;
	// loop for 0.5*w*h times
	for(i = 0; i < height; i++)
		for(j = 0; j < width; j+=2)
		{
			int
				y1 = *inbuf--,
				u = *inbuf-- - 128,
				y2 = *inbuf--,
				v = *inbuf-- - 128;
			char rgb12[] = {
				_Clamp(y1 + 1.402*v),
				_Clamp(y1 - 0.344*u - 0.71414*v),
				_Clamp(y1 + 1.772*u),
				_Clamp(y2 + 1.402*v),
				_Clamp(y2 - 0.344*u - 0.71414*v),
				_Clamp(y2 + 1.772*u)
			};
			write(outfilefd, rgb12, 6);
		}
	return 0;
}

// size: total input buffer size in byte
void YUV422ToRGB(char *outbuf, char *inbuf, int size)
{
	int i;
	for(i = 0; i < size; i+=4)
	{
		char
			y1 = *inbuf++,
			u = *inbuf++ - 128,
			y2 = *inbuf++,
			v = *inbuf++ - 128;
		char rgb12[] = {
			(y1 + 1.402*v),
			(y1 - 0.344*u - 0.71414*v),
			(y1 + 1.772*u),
			(y2 + 1.402*v),
			(y2 - 0.344*u - 0.71414*v),
			(y2 + 1.772*u)
		};
		memcpy(outbuf, rgb12, 6);
		outbuf += 6;
	}
}
