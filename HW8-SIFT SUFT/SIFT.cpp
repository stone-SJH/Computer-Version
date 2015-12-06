#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<vector>
using namespace std;
using namespace cv;

int main(){
	Mat src = imread("D:\\pictures\\test2.png");
	Mat src2 = imread("D:\\pictures\\test5.jpg");

	SiftFeatureDetector  sift;
	vector<KeyPoint>kp1, kp2;

	sift.detect(src, kp1);
	Mat outsrc1;
	drawKeypoints(src, kp1, outsrc1);
	imshow("image1 keypoints", outsrc1);
	sift.detect(src2, kp2);
	Mat outsrc2;
	drawKeypoints(src2, kp2, outsrc2);
	imshow("image2 keypoints", outsrc2);


	SiftDescriptorExtractor extractor;
	Mat descriptor1, descriptor2;
	BruteForceMatcher<L2<float>> matcher;
	vector<DMatch> matches;
	Mat src_matches;
	
	extractor.compute(src, kp1, descriptor1);
	extractor.compute(src2, kp2, descriptor2);


	imshow("descriptor", descriptor1);
	//cout << endl << descriptor1 << endl;
	matcher.match(descriptor1, descriptor2, matches);

	drawMatches(src, kp1, src2, kp2, matches, src_matches);
	imshow("matches", src_matches);

	waitKey();
	return 0;
}