#include"opencv.h"

void calSlope(Mat src, Mat& direction, Mat& magnitude){
	Mat magX = Mat(src.rows, src.cols, CV_32F);
	Mat magY = Mat(src.rows, src.cols, CV_32F);
	Sobel(src, magX, CV_32F, 1, 0, 3);
	Sobel(src, magY, CV_32F, 0, 1, 3);

	direction = Mat(src.rows, src.cols, CV_32F);
	divide(magY, magX, direction);

	Mat mag2X = Mat(src.rows, src.cols, CV_64F);
	Mat mag2Y = Mat(src.rows, src.cols, CV_64F);
	multiply(magX, magX, mag2X);
	multiply(magY, magY, mag2Y);
	sqrt(mag2X + mag2Y, magnitude);
}
void nonMaximumSuppression(Mat &magnitudeImage, Mat &directionImage) {
	Mat checkImage = Mat(magnitudeImage.rows, magnitudeImage.cols, CV_8U);

	MatIterator_<float>itMag = magnitudeImage.begin<float>();
	MatIterator_<float>itEnd = magnitudeImage.end<float>();
	MatIterator_<float>itDirection = directionImage.begin<float>();
	MatIterator_<unsigned char>itRet = checkImage.begin<unsigned char>();

	for (; itMag != itEnd; ++itDirection, ++itRet, ++itMag) {
		const Point pos = itRet.pos();
		float currentDirection = atan(*itDirection) * (180 / PI);
		*itDirection = currentDirection;
		if ((currentDirection > 22.5 && currentDirection <= 67.5) ||
			(currentDirection > -157.5 && currentDirection <= -112.5)) {
			if (pos.y > 0 && pos.x > 0 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x - 1)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;
			}
			if (pos.y < magnitudeImage.rows - 1 && pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x + 1)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;
			}
		}
		else if ((currentDirection > 67.5 && currentDirection <= 112.5) ||
				(currentDirection > -112.5 && currentDirection <= -67.5)){
			if (pos.y > 0 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;
			}
			if (pos.y<magnitudeImage.rows - 1 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;
			}

		}
		else if ((currentDirection > 112.5 && currentDirection <= 157.5) ||
				(currentDirection > -67.5 && currentDirection <= -22.5)){
			if (pos.y>0 && pos.x<magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y - 1, pos.x + 1)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;;
			}
			if (pos.y < magnitudeImage.rows - 1 && pos.x>0 && *itMag <= magnitudeImage.at<float>(pos.y + 1, pos.x - 1)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;
			}
		}
		else {
			if (pos.x > 0 && *itMag <= magnitudeImage.at<float>(pos.y, pos.x - 1)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;
			}
			if (pos.x < magnitudeImage.cols - 1 && *itMag <= magnitudeImage.at<float>(pos.y, pos.x + 1)) {
				magnitudeImage.at<float>(pos.y, pos.x) = 0;
			}
		}
	}
}


void followEdges(int x, int y, Mat magnitude, int up, int low, Mat &edges) {
	edges.at<float>(x, y) = 255;

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if ((i != 0) && (j != 0) && (x + i >= 0) && (y + j >= 0) && (x + i < magnitude.rows) && (y + j < magnitude.cols)){
				if ((magnitude.at<float>(x + i, y + j) > up) && (edges.at<float>(x + i, y + j) != 255)) {
					followEdges(x + i, y + j, magnitude, up, low, edges);
				}
			}
		}
	}
}

void hysteresis(Mat magnitude, int up, int low, Mat& edgesImage) {
	int width = magnitude.rows;
	int height = magnitude.cols;

	edgesImage = Mat(magnitude.rows, magnitude.cols, magnitude.type());

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (magnitude.at<float>(x, y) >= up){
				followEdges(x, y, magnitude, up, low, edgesImage);
			}
		}
	}
}

void cannyEdgeDetection(Mat &src, Mat &edgesImage, int upperThresh, int lowerThresh) {
	Mat image = src.clone();;
	//step1:ʹ�ø�˹�˲�����������ȥ��
	GaussianBlur(src, image, Size(3, 3), 1.4);
	//step2:ʹ��Sobel���Ӽ����Ե�ݶ�ֵ���ݶȷ���
	Mat direction;
	Mat magnitude;
	calSlope(image, direction, magnitude);
	//step3:���з�����ݶȵ�ֵ����
	nonMaximumSuppression(magnitude, direction);
	//step4:ʹ��hysteresis˫��ֵ�㷨���б�Ե����
	hysteresis(magnitude, upperThresh, lowerThresh, edgesImage);
	//����step3,step4��Ϊ������opencv���к�����ɣ�����ʵ���㷨��������ĵ�

}


int main(){
	Mat src = imread("D:\\pictures\\test2.png", 0);
	imshow("origin", src);
	int up = 85;
	int low = 30;

	Mat dst;
	cannyEdgeDetection(src, dst, up, low);

	
	imshow("edge", dst);
	waitKey();
	return 0;
}
