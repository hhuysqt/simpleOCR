#include "CWindow.h"
#include <iostream>
#include <string.h>
#include <cmath>

CWindow::CWindow(char *maskfilename):framecnt(0)
{
	// init camera
	CameraInit(CameraInitFmtYUYV);
	width = CameraGetWidth();
	height = CameraGetHeight();

	// init image
	cameraImg = new QImage(width, height, QImage::Format_RGB888);
	cameraLumImg = new QImage(width, height, QImage::Format_RGB888);
	maskImg = new QImage(maskfilename);
	maskLumImg = new QImage(width, height, QImage::Format_RGB888);

	// init window
	camimglbl = new QLabel;
	camlumlbl = new QLabel;
	maskimglbl = new QLabel;
	masklumlbl = new QLabel;
	camimglbl->setPixmap(QPixmap::fromImage(*cameraImg));
	camlumlbl->setPixmap(QPixmap::fromImage(*cameraLumImg));
	maskimglbl->setPixmap(QPixmap::fromImage(*maskImg));
	masklumlbl->setPixmap(QPixmap::fromImage(*maskLumImg));
	resultlabel = new QLabel("result");

	imggridlayout = new QGridLayout;
	imggridlayout->addWidget(camimglbl, 0, 0);
	imggridlayout->addWidget(camlumlbl, 0, 1);
	imggridlayout->addWidget(maskimglbl, 1, 0);
	imggridlayout->addWidget(masklumlbl, 1, 1);

	mainlayout = new QVBoxLayout;
	mainlayout->addLayout(imggridlayout);
	mainlayout->addWidget(resultlabel);
	this->setLayout(mainlayout);
	this->resize(width*2, height*2 + 100);

	// trigger update event to draw picture
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(1000);
}

void CWindow::fillCameraPixel()
{
	CameraShoot();
	void *pRawdata;
	int picsize = CameraGetPicture(&pRawdata);
	unsigned char *pImgdata = cameraImg->scanLine(0);
	YUV422ToRGB((char*)pImgdata, (char*)pRawdata, picsize);
}

void CWindow::paintEvent(QPaintEvent *event)
{
	(void)event;
	static QPainter paint(this);
	static QString str;
	framecnt++;

	fillCameraPixel();
	transferLuminance();
	camimglbl->setPixmap(QPixmap::fromImage(*cameraImg));
	camlumlbl->setPixmap(QPixmap::fromImage(*cameraLumImg));
	maskimglbl->setPixmap(QPixmap::fromImage(*maskImg));
	masklumlbl->setPixmap(QPixmap::fromImage(*maskLumImg));
	resultlabel->setText(str.setNum(calcSimilarity()));
	//paint.begin(this);
	//paint.drawImage(0, 0, *maskImg);
	//paint.drawImage(width, 0, *cameraImg);
	//paint.drawImage(0, height, *maskLumImg);
	//paint.drawImage(width, height, *cameraLumImg);
	//paint.end();

	//std::cout << "\t" << calcSimilarity() << std::endl;
}

void CWindow::transferLuminance()
{
	unsigned char *maskData = maskLumImg->scanLine(0),
		      *camData = cameraLumImg->scanLine(0),
		      *maskSrc = maskImg->scanLine(0),
		      *camSrc = cameraImg->scanLine(0);
	int factor = averagedarkness(camSrc);

	for(int i = 0; i < width*height; i++)
	{
		unsigned char rgbmask[3], rgbcam[3];
		memcpy(rgbmask, maskSrc, 3);
		memcpy(rgbcam, camSrc, 3);
		unsigned char
		masklum = 255 - (0.299*rgbmask[0] + 0.587*rgbmask[1] + 0.114*rgbmask[2]),
		camlum = clamp(255 - (0.299*rgbcam[0] + 0.587*rgbcam[1] + 0.114*rgbcam[2]), 0.51);
		memset(maskData, masklum, 3);
		memset(camData, camlum, 3);

		// qt changes the bmp format to ARGB!!!
		// so maskSrc has to be increased by 4
		maskData += 3, maskSrc += 4;
		camData += 3, camSrc += 3;
	}
}

float CWindow::calcSimilarity()
{
	int result = 0;
	int maskdotsum = 0;
	signed char *maskData = (signed char*)maskLumImg->scanLine(0),
		    *camData = (signed char*)cameraLumImg->scanLine(0);
	for(int i = 0; i < width*height; i++)
	{
		signed int curmaskval = (signed)maskData[0] - 128;
		maskdotsum += maskData[0];
		result += curmaskval * camData[0];
		maskData += 3, camData += 3;
	}
	return 1.0 * result / maskdotsum;
}

int CWindow::clamp(float darkness, float factor)
{
	return 255.0/(1 + exp(-factor * (darkness-127)));
}

int CWindow::averagedarkness(unsigned char *pimg)
{
	int result = 0;
	for(int i = 0; i < width*height; i++)
	{
		result += pimg[i*3];
	}
	return result/(width*height*10);
}
