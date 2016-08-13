#include "bmp.h"

void BMPFillHeader(pBITMAPFILEHEADER pbmph, pBITMAPINFOHEADER pbmpi, int width, int height)
{
	pbmph->bfType = 0x4d42;
	pbmph->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width*height*3;
	pbmph->bfReserved1 = pbmph->bfReserved2 = 0;
	pbmph->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	pbmpi->biSize = sizeof(BITMAPINFOHEADER);
	pbmpi->biBitCount = 24;
	pbmpi->biWidth = width;
	pbmpi->biHeight = height;
	pbmpi->biSizeImage = width*height*3;
	pbmpi->biClrUsed = 0;
	pbmpi->biClrImportant = 0;
	pbmpi->biCompression = 0;
	pbmpi->biPlanes = 1;
	pbmpi->biXPelsPerMeter = 0x00000ec4;
	pbmpi->biYPelsPerMeter = 0x00000ec4;
}

void* BMPGetDataStartAddr(void *pbmp)
{
	pBITMAPFILEHEADER pbmph = pbmp;
	return (void*)((uint64_t)pbmp + pbmph->bfOffBits);
}
int BMPGetDataSize(void *pbmp)
{
	pBITMAPFILEHEADER pbmph = pbmp;
	return pbmph->bfSize - pbmph->bfOffBits;
}

void BMPCalcLuminance(unsigned char *poutdata, unsigned char *pbmpdata, int picsize)
{
	picsize /= 3;
	while(picsize--)
	{
		unsigned char r = *pbmpdata++, g = *pbmpdata++, b = *pbmpdata++;
		*poutdata++ = 0.299*r + 0.587*g + 0.114*b;
	}
}
