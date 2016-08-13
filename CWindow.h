#ifndef MY_CWINDOW_H
#define MY_CWINDOW_H

#include <QtGui>
#include "yuv2bmp.h"
#include "bmp.h"
#include "CameraDriver.h"

class CWindow : public QWidget
{
Q_OBJECT
public:
	CWindow(char *maskfilename);
	void fillCameraPixel();
private slots:
	void paintEvent(QPaintEvent *event);
private:
	int width, height;
	QLabel *camimglbl, *camlumlbl, *maskimglbl, *masklumlbl;
	QImage *cameraImg, *cameraLumImg;
	QImage *maskImg, *maskLumImg;
	int framecnt;
	QTimer *timer;
	QGridLayout *imggridlayout;
	QVBoxLayout *mainlayout;
	QLabel *resultlabel;

	// the main OCR impliments
	void transferLuminance();
	float calcSimilarity();
	int clamp(float darkness, float factor);
	int averagedarkness(unsigned char *pimg);
};

#endif

