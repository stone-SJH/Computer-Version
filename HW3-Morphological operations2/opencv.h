#ifndef OPENCVH
#define OPENCVH

#include <opencv2\opencv.hpp>
#include <highgui.h>
#include <cv.h>
#include <iostream>
#include <string>
#include <cmath>
#include <math.h>
#include <float.h>
#include "opencv2/imgproc/imgproc.hpp"


#define TYPE_MORPH_RECT      (0)
#define TYPE_MORPH_CROSS     (1)
#define TYPE_MORPH_ELLIPSE   (2)

#define MAX_ELE_TYPE         (2)
#define MAX_ELE_SIZE         (20)



using namespace cv;
using namespace std;

int otsu(Mat dst);
#endif