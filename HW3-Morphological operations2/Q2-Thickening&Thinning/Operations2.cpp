#include "opencv.h"

void Boundary(Mat src){
	Mat dst;
	//3*3 element
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
	erode(src, dst, ele);
	dst = src - dst;

	imshow("boundary", dst);
}
Mat HitOrMiss(Mat src, Mat Kernel1){
	int threhold = 180;//若为灰度图则应用OTSU获取阈值
	threshold(Kernel1, Kernel1, threhold, 255, CV_THRESH_BINARY);
	Mat Blankimage = Mat::ones(Kernel1.rows, Kernel1.cols, CV_8UC1);
	Mat Kernel_S2 = Blankimage * 255 - Kernel1;

	Mat hit_result, hit_result1, hit_result2;
	erode(src, hit_result1, Kernel1, Point(-1, -1), 1, BORDER_DEFAULT, 0);

	Mat BigBlankimage = Mat::ones(src.rows, src.cols, CV_8UC1);
	Mat reverse = BigBlankimage * 255 - src;
	erode(reverse, hit_result2, Kernel_S2, Point(-1, -1), 1, BORDER_DEFAULT, 0);

	hit_result = hit_result1 & hit_result2;
	//imshow("hit-or-miss", hit_result);
	return hit_result;
}

void Thinning(Mat src, Mat Kernel){
	Mat hit_result = HitOrMiss(src, Kernel);
	Mat BigBlankimage = Mat::ones(src.rows, src.cols, CV_8UC1);
	Mat thinning_result;
	thinning_result = BigBlankimage * 255 - hit_result;
	thinning_result = src & thinning_result;
	imshow("thinning", thinning_result);
}

void Thickening(Mat src, Mat Kernel){
	Mat hit_result = HitOrMiss(src, Kernel);
	Mat thicking_result;
	thicking_result = src | hit_result;
	imshow("thicking", thicking_result);
}

int main(){
	
	Mat src = imread("D:\\pictures\\test2.png");
	cvtColor(src, src, CV_RGB2GRAY);
	//int threhold = otsu(src);
	//cout << threshold;
	imshow("origin", src);
	//threshold(src, src, threhold, 255, CV_THRESH_BINARY);
	//imshow("2", src);
	//Boundary(src);
	//3*3 Elements
	Mat Kernel1 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
	//Mat Kernel_S1 = imread("D:\\pictures\\rice.png");
	//cvtColor(Kernel_S1, Kernel_S1, CV_RGB2GRAY);
	
	Thinning(src, Kernel1);
	Thickening(src, Kernel1);
	waitKey();
	return 0;
}