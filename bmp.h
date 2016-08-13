#ifndef _MY_BMP_H_
#define _MY_BMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
// data structure
#pragma pack(1)  
typedef struct BITMAPFILEHEADER  
{  
	uint16_t bfType;//位图文件的类型,  
	uint32_t bfSize;//位图文件的大小，以字节为单位  
	uint16_t bfReserved1;//位图文件保留字，必须为0  
	uint16_t bfReserved2;//同上  
	uint32_t bfOffBits;//位图阵列的起始位置，以相对于位图文件   或者说是头的偏移量表示，以字节为单位  
} BITMAPFILEHEADER, *pBITMAPFILEHEADER;
#pragma pack()  


typedef struct BITMAPINFOHEADER//位图信息头类型的数据结构，用于说明位图的尺寸  
{  
	uint32_t biSize;//位图信息头的长度，以字节为单位  
	uint32_t biWidth;//位图的宽度，以像素为单位  
	uint32_t biHeight;//位图的高度，以像素为单位  
	uint16_t biPlanes;//目标设备的级别,必须为1  
	uint16_t biBitCount;//每个像素所需的位数，必须是1(单色),4(16色),8(256色)或24(2^24色)之一  
	uint32_t biCompression;//位图的压缩类型，必须是0-不压缩，1-BI_RLE8压缩类型或2-BI_RLE4压缩类型之一  
	uint32_t biSizeImage;//位图大小，以字节为单位  
	uint32_t biXPelsPerMeter;//位图目标设备水平分辨率，以每米像素数为单位  
	uint32_t biYPelsPerMeter;//位图目标设备垂直分辨率，以每米像素数为单位  
	uint32_t biClrUsed;//位图实际使用的颜色表中的颜色变址数  
	uint32_t biClrImportant;//位图显示过程中被认为重要颜色的变址数  
} BITMAPINFOHEADER, *pBITMAPINFOHEADER;

// 填充BMP头的两个数据结构
void BMPFillHeader(pBITMAPFILEHEADER pbmph, pBITMAPINFOHEADER pbmpi, int width, int height);
// 获取BMP的具体数据区域
void* BMPGetDataStartAddr(void *pbmp);
// 获取BMP图像数据大小
int BMPGetDataSize(void *pbmp);
// 计算灰度值，存入int数组中
void BMPCalcLuminance(unsigned char *poutdata, unsigned char *pbmpdata, int size);

#ifdef __cplusplus
}
#endif

#endif
