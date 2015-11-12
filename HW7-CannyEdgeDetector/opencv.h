/*stone's opencv header*/
#ifndef OPENCVH
#define OPENCVH

#include <opencv2\opencv.hpp>
#include <highgui.h>
#include <cv.h>
#include <iostream>
#include <string>
#include <cmath>
#include "cxcore.h"
#include <math.h>
#include <float.h>
#include "opencv2/imgproc/imgproc.hpp"
#define PI 3.14
//Operations1.cpp
#define TYPE_MORPH_RECT      (0)
#define TYPE_MORPH_CROSS     (1)
#define TYPE_MORPH_ELLIPSE   (2)
#define MAX_ELE_TYPE         (2)
#define MAX_ELE_SIZE         (20)

//HistogramEqualization.cpp
#define HISTOGRAM_SIZE 256
#define MAX_INTENSITY 255
//Skeleton.cpp
static unsigned char deletemark[256] = {
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1,
	0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
	1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0,
	0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0,
	1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0
};//Ë÷Òý±í  

using namespace cv;
using namespace std;

Mat Otsu(Mat src);
Mat Boundary(Mat src);
Mat HitOrMiss(Mat src, Mat Kernel1);
Mat Thinning(Mat src, Mat Kernel);
void Thickening(Mat src, Mat Kernel);
Mat HistEqual(Mat src);
int otsu(Mat dst);

#endif