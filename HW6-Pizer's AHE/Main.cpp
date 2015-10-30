#include "cv.h" 
#include "highgui.h"

#include "clahe.h"

#include <stdio.h>

int main(){
	int xdivs = 2;//2-16 and must be divided by width
	int ydivs = 2;//2-16 and must be divided by height
	int bins = 256;//2-256 --number of histogram bins to use per division
	int limit_counter = 14;//contrast limit for localised changes in contrast 

	IplImage* img = cvLoadImage("D:\\pictures\\test2.png", 1);
	IplImage* grayImg =
		cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
	grayImg->origin = img->origin;
	IplImage* eqImg =
		cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
	eqImg->origin = img->origin;
	if (img->nChannels > 1){
		cvCvtColor(img, grayImg, CV_BGR2GRAY);
	}
	else {
		grayImg = img;
	}
	cvShowImage("origin", grayImg);
	cvCLAdaptEqualize(grayImg, eqImg, (unsigned int)xdivs, (unsigned int)ydivs,
		(unsigned int)bins, (float)limit_counter * 0.1, CV_CLAHE_RANGE_FULL);
	cvShowImage("after", eqImg);
	
	cvWaitKey();
}
