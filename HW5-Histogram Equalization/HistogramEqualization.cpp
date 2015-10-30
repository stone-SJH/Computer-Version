#include "opencv.h"

Mat HistEqual(Mat src){
	long histogram[HISTOGRAM_SIZE] = { 0 };
	long sumHistogram[HISTOGRAM_SIZE] = { 0 };
	int height = src.cols;
	int width = src.rows;
	Mat outImage = src;

	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			int tmp = src.at<uchar>(i, j);
			histogram[tmp]++;
		}
	}

	long sum = 0;
	for (int i = 0; i < HISTOGRAM_SIZE; ++i) {
		sum += histogram[i];
		sumHistogram[i] = sum;
	}

	long pixelCount = width * height;
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			int gray = src.at<uchar>(i, j);
			outImage.at<uchar>(i, j) = sumHistogram[gray] * MAX_INTENSITY / pixelCount;
		}
	}

	return outImage;
}

int main(){
	Mat src = imread("D:\\pictures\\test2.png");
	cvtColor(src, src, CV_RGB2GRAY);
	imshow("origin", src);
	Mat dst = HistEqual(src);
	imshow("after enhancement", dst);
	waitKey();
	return 0;
}