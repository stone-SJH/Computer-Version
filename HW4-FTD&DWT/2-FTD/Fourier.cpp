#include "opencv.h"

Mat Fourier(Mat src){
	Mat padded;
	int m = getOptimalDFTSize(src.rows);
	int n = getOptimalDFTSize(src.cols);

	copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);
	dft(complexI, complexI);
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))  
	split(complexI, planes);
	// planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))  
	magnitude(planes[0], planes[1], planes[0]);

	Mat magI = planes[0];
	magI += Scalar::all(1);
	log(magI, magI);
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	//重新设置四象限位置以使得原点在中心位置      
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;
	Mat q0(magI, Rect(0, 0, cx, cy));
	Mat q1(magI, Rect(cx, 0, cx, cy));
	Mat q2(magI, Rect(0, cy, cx, cy));
	Mat q3(magI, Rect(cx, cy, cx, cy));
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(magI, magI, 0, 1, CV_MINMAX);
	return magI;
}

int main(){
	Mat src = imread("D:\\pictures\\test1.png", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("src", src);
	Mat dst = Fourier(src);
	imshow("spectrum", dst);
	waitKey();
	return 0;
}