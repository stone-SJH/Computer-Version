#include "opencv.h"

#define MAX_GRAY_VALUE 256
#define MIN_GRAY_VALUE 0

int Fuzzy_Linear_Index(Mat src){
	int histogram[256] = { 0 };
	int height = src.cols;
	int width = src.rows;

	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			int tmp = src.at<uchar>(i, j);
			histogram[tmp]++;
		}
	}
	int First, Last, Y;
	for (First = 0; First < 256 && histogram[First] == 0; First++);
	for (Last = 255; Last > First && histogram[Last] == 0; Last--);
	if (First == Last || First + 1 == Last) return First;

	long* S = new long[Last + 1];
	long* W = new long[Last + 1];
	for (int i = 0; i <= Last; i++){
		S[i] = 0;
		W[i] = 0;
	}
	for (Y = First > 1 ? First : 1; Y <= Last; Y++)
	{
		S[Y] = S[Y - 1] + histogram[Y];
		W[Y] = W[Y - 1] + Y * histogram[Y];
	}
	int Threshold;
	double MaxLIndex = DBL_MAX;
	for (Y = First; Y < Last; Y++){
		float m0 = W[Y] / S[Y];
		float m1 = (W[Last] - W[Y]) / (S[Last] - S[Y]);
		int c = Last - First;
		double LIndex = 0;
		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){
				float ma;
				if (src.at<uchar>(i, j) < Y)
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m1));
				else
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m0));//C为常数，在比较中可以忽略
				LIndex += min(ma, 1 - ma);
			}
		}
		LIndex = LIndex * 2; //n
		if (LIndex < MaxLIndex){
			MaxLIndex = LIndex;//min
			Threshold = Y;
		}
	}
	return Threshold;
}

int Fuzzy_Quadratic_Index(Mat src){
	int histogram[256] = { 0 };
	int height = src.cols;
	int width = src.rows;

	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			int tmp = src.at<uchar>(i, j);
			histogram[tmp]++;
		}
	}
	int First, Last, Y;
	for (First = 0; First < 256 && histogram[First] == 0; First++);
	for (Last = 255; Last > First && histogram[Last] == 0; Last--);
	if (First == Last || First + 1 == Last) return First;

	long* S = new long[Last + 1];
	long* W = new long[Last + 1];
	for (int i = 0; i <= Last; i++){
		S[i] = 0;
		W[i] = 0;
	}
	for (Y = First > 1 ? First : 1; Y <= Last; Y++)
	{
		S[Y] = S[Y - 1] + histogram[Y];
		W[Y] = W[Y - 1] + Y * histogram[Y];
	}
	int Threshold;
	double MaxLIndex = DBL_MAX;
	for (Y = First; Y < Last; Y++){
		float m0 = W[Y] / S[Y];
		float m1 = (W[Last] - W[Y]) / (S[Last] - S[Y]);
		int c = Last - First;
		double LIndex = 0;
		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){
				float ma;
				if (src.at<uchar>(i, j) < Y)
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m1));
				else
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m0));//C为常数，在比较中可以忽略
				LIndex += pow(min(ma, 1 - ma), 2);
			}
		}
		LIndex = sqrt(LIndex) * 2; //n
		if (LIndex < MaxLIndex){
			MaxLIndex = LIndex;//min
			Threshold = Y;
		}
	}
	return Threshold;
}

int Fuzzy_Similarity(Mat src){
	int histogram[256] = { 0 };
	int height = src.cols;
	int width = src.rows;

	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			int tmp = src.at<uchar>(i, j);
			histogram[tmp]++;
		}
	}
	int First, Last, Y;
	for (First = 0; First < 256 && histogram[First] == 0; First++);
	for (Last = 255; Last > First && histogram[Last] == 0; Last--);
	if (First == Last || First + 1 == Last) return First;

	long* S = new long[Last + 1];
	long* W = new long[Last + 1];
	for (int i = 0; i <= Last; i++){
		S[i] = 0;
		W[i] = 0;
	}
	for (Y = First > 1 ? First : 1; Y <= Last; Y++)
	{
		S[Y] = S[Y - 1] + histogram[Y];
		W[Y] = W[Y - 1] + Y * histogram[Y];
	}
	int Threshold;
	double MinLIndex = DBL_MIN;
	for (Y = First; Y < Last; Y++){
		float m0 = W[Y] / S[Y];
		float m1 = (W[Last] - W[Y]) / (S[Last] - S[Y]);
		int c = Last - First;
		double LIndex = 0;
		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){
				float ma;
				if (src.at<uchar>(i, j) < Y)
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m0));
				else
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m1));//C为常数，在比较中可以忽略
				LIndex += ma;
			}
		}
		LIndex = LIndex; //n
		if (LIndex > MinLIndex){
			MinLIndex = LIndex;//max
			Threshold = Y;
		}
	}
	return Threshold;
}

int Fuzzy_Compactness(Mat src){
	int histogram[256] = { 0 };
	int height = src.cols;
	int width = src.rows;

	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			int tmp = src.at<uchar>(i, j);
			histogram[tmp]++;
		}
	}
	int First, Last, Y;
	for (First = 0; First < 256 && histogram[First] == 0; First++);
	for (Last = 255; Last > First && histogram[Last] == 0; Last--);
	if (First == Last || First + 1 == Last) return First;

	long* S = new long[Last + 1];
	long* W = new long[Last + 1];
	for (int i = 0; i <= Last; i++){
		S[i] = 0;
		W[i] = 0;
	}
	for (Y = First > 1 ? First : 1; Y <= Last; Y++)
	{
		S[Y] = S[Y - 1] + histogram[Y];
		W[Y] = W[Y - 1] + Y * histogram[Y];
	}
	int Threshold;
	double MinLIndex = DBL_MIN;
	float** mu = new float* [width];
	for (int i = 0; i < width; i++)
		mu[i] = new float[height];

	for (Y = First; Y < Last; Y++){
		float m0 = W[Y] / S[Y];
		float m1 = (W[Last] - W[Y]) / (S[Last] - S[Y]);
		int c = Last - First;
		double Index = 0;
		double A = 0; //A(μ)
		double P = 0; //P(μ)

		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				mu[i][j] = 0;

		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){
				float ma;
				if (src.at<uchar>(i, j) < Y)
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m1));
				else
					ma = exp(-1 * log(2) * abs(src.at<uchar>(i, j) - m0));//C为常数，在比较中可以忽略
				mu[i][j] = ma;
			}
		}

		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				A += mu[i][j];

		for (int i = 0; i < width; i++)
			for (int j = 0; j < height - 1; j++)
				P += abs(mu[i][j] - mu[i][j + 1]);
		for (int i = 0; i < width - 1; i++)
			for (int j = 0; j < height; j++)
				P += abs(mu[i][j] - mu[i + 1][j]);

		Index = A / (pow(P, 2));

		if (Index > MinLIndex){
			MinLIndex = Index;//max
			Threshold = Y;
		}
	}
	return Threshold;
}

int Fuzzy_Huang(Mat src){
	int X, Y;
	int First, Last;
	int Threshold = -1;
	double BestEntropy = DBL_MAX, Entropy;

	int histogram[256] = { 0 };
	int height = src.cols;
	int width = src.rows;

	for (int i = 0; i<width; i++){
		for (int j = 0; j<height; j++){
			int tmp = src.at<uchar>(i, j);
			histogram[tmp]++;
		}
	}

	for (First = 0; First < 256 && histogram[First] == 0; First++);
	for (Last = 255; Last > First && histogram[Last] == 0; Last--);
	if (First == Last || First + 1 == Last) return First;

	long* S = new long[Last + 1];
	long* W = new long[Last + 1];
	for (int i = 0; i <= Last; i++){
		S[i] = 0;
		W[i] = 0;
	}
	for (Y = First > 1 ? First : 1; Y <= Last; Y++)
	{
		S[Y] = S[Y - 1] + histogram[Y];
		W[Y] = W[Y - 1] + Y * histogram[Y];
	}

	//minimum entropy
	for (Y = First; Y < Last; Y++)
	{
		Entropy = 0;
		float m0 = W[Y] / S[Y]; 
		float m1 = (W[Last] - W[Y]) / (S[Last] - S[Y]);
		for (int i = 0; i < width; i++){
			for (int j = 0; j < height; j++){	
				double ma = 0;
				if (src.at<uchar>(i, j) < Y)
					ma = exp(-1 * log(2) * 2 * abs(src.at<uchar>(i, j) - m1));
				else
					ma = exp(-1 * log(2) * 2 * abs(src.at<uchar>(i, j) - m0));
				Entropy += -1 * ma * log(ma) / log(2) - (1 - ma) * log(1 - ma) / log(2);
			}
		}
		if (BestEntropy > Entropy)
		{
			BestEntropy = Entropy;
			Threshold = Y;
		}
	}
	return Threshold;
}


int otsu(Mat dst){
	double maxcov = 0.0;
	int maxthread = 0;

	int hst[MAX_GRAY_VALUE] = { 0 };
	double pro_hst[MAX_GRAY_VALUE] = { 0.0 };

	int height = dst.cols;
	int width = dst.rows;

	for (int i = 0; i<width; i++){
		for (int j = 0; j<height; j++){
			int tmp = dst.at<uchar>(i, j);
			hst[tmp]++;
		}
	}

	for (int i = MIN_GRAY_VALUE; i<MAX_GRAY_VALUE; i++)
		pro_hst[i] = (double)hst[i] / (double)(width*height);

	double u = 0;
	for (int i = MIN_GRAY_VALUE; i<MAX_GRAY_VALUE; i++)
		u += i*pro_hst[i];
	double cov;
	double det = 0.0;
	double u0, u1, w0, w1, uk;
	for (int i = MIN_GRAY_VALUE; i< MAX_GRAY_VALUE; i++)
		det += (i - u)*(i - u)*pro_hst[i];

	for (int i = MIN_GRAY_VALUE; i<MAX_GRAY_VALUE; i++){

		w0 = 0.0; w1 = 0.0; u0 = 0.0; u1 = 0.0; uk = 0.0;

		for (int j = MIN_GRAY_VALUE; j < i; j++){

			uk += j*pro_hst[j];
			w0 += pro_hst[j];

		}
		u0 = uk / w0;
		w1 = 1 - w0;
		u1 = (u - uk) / (1 - w0);
		cov = w0*w1*(u1 - u0)*(u1 - u0);
		if (cov > maxcov){
			maxcov = cov;//max
			maxthread = i;
		}
	}
	return maxthread;


}
Mat Otsu(Mat src){
	double maxcov = 0.0;
	int maxthread = 0;

	int hst[MAX_GRAY_VALUE] = { 0 };
	double pro_hst[MAX_GRAY_VALUE] = { 0.0 };

	int height = src.cols;
	int width = src.rows;

	for (int i = 0; i<width; i++){
		for (int j = 0; j<height; j++){
			int tmp = src.at<uchar>(i, j);
			hst[tmp]++;
		}
	}

	for (int i = MIN_GRAY_VALUE; i<MAX_GRAY_VALUE; i++)
		pro_hst[i] = (double)hst[i] / (double)(width*height);

	double u = 0;
	for (int i = MIN_GRAY_VALUE; i<MAX_GRAY_VALUE; i++)
		u += i*pro_hst[i];
	double cov;
	double det = 0.0;
	double u0, u1, w0, w1, uk;
	for (int i = MIN_GRAY_VALUE; i< MAX_GRAY_VALUE; i++)
		det += (i - u)*(i - u)*pro_hst[i];

	for (int i = MIN_GRAY_VALUE; i<MAX_GRAY_VALUE; i++){

		w0 = 0.0; w1 = 0.0; u0 = 0.0; u1 = 0.0; uk = 0.0;

		for (int j = MIN_GRAY_VALUE; j < i; j++){

			uk += j*pro_hst[j];
			w0 += pro_hst[j];

		}
		u0 = uk / w0;
		w1 = 1 - w0;
		u1 = (u - uk) / (1 - w0);
		cov = w0*w1*(u1 - u0)*(u1 - u0);
		if (cov > maxcov){
			maxcov = cov;//max
			maxthread = i;
		}
	}

	int threshold = maxthread;
	Mat dst;
	cvtColor(src, dst, CV_RGB2GRAY);
	for (int i = 0; i < width; i++)
		for (int j = 0; j< height; j++)
			if (dst.at<uchar>(i, j) > threshold)
				dst.at<uchar>(i, j) = MAX_GRAY_VALUE - 1;
			else
				dst.at<uchar>(i, j) = MIN_GRAY_VALUE;
	return dst;
}
//main
/*
int main(){
	cout << log(2) << endl;

	int width, height;
	int i, j;
	cv::Mat obj = cv::imread("D:\\pictures\\test2.png");
	cv::Mat dst;
	cv::cvtColor(obj, dst, CV_RGB2GRAY);
	cv::imshow("origin img", dst);

	height = dst.cols;
	width = dst.rows;

	cv::Mat dst2;
	cv::cvtColor(obj, dst2, CV_RGB2GRAY);
	int thd2 = otsu(dst2);
	cout <<"otsu 阈值："<< thd2 << endl;
	for (i = 0; i < width; i++)
		for (j = 0; j< height; j++)
			if (dst2.at<uchar>(i, j) > thd2)
				dst2.at<uchar>(i, j) = MAX_GRAY_VALUE - 1;
			else
				dst2.at<uchar>(i, j) = MIN_GRAY_VALUE;
	cv::imshow("img after ostu", dst2);
	imwrite("D:\\result\\test2-otsu.png", dst2);
	
	int thd = Fuzzy_Linear_Index(dst);
	cout << "fuzzy linear index 阈值："<< thd << endl;
	for (i = 0; i < width; i++)
		for (j = 0; j< height; j++)
			if (dst.at<uchar>(i, j) > thd)
				dst.at<uchar>(i, j) = MAX_GRAY_VALUE - 1;
			else
				dst.at<uchar>(i, j) = MIN_GRAY_VALUE;
	cv::imshow("img after fuzzy-linear-index", dst);
	imwrite("D:\\result\\test2-fuzzy-linear-index.png", dst);

	cv::Mat dst3;
	cv::cvtColor(obj, dst3, CV_RGB2GRAY);
	int thd3 = Fuzzy_Quadratic_Index(dst3);
	cout << "fuzzy quadratic index 阈值：" << thd3 << endl;
	for (i = 0; i < width; i++)
		for (j = 0; j< height; j++)
			if (dst3.at<uchar>(i, j) > thd3)
				dst3.at<uchar>(i, j) = MAX_GRAY_VALUE - 1;
			else
				dst3.at<uchar>(i, j) = MIN_GRAY_VALUE;
	cv::imshow("img after fuzzy-quadratic-index", dst3);
	imwrite("D:\\result\\test2-fuzzy-quadratic-index.png", dst3);

	cv::Mat dst4;
	cv::cvtColor(obj, dst4, CV_RGB2GRAY);
	int thd4 = Fuzzy_Similarity(dst4);
	cout << "fuzzy similarity 阈值：" << thd4 << endl;
	for (i = 0; i < width; i++)
		for (j = 0; j< height; j++)
			if (dst4.at<uchar>(i, j) > thd4)
				dst4.at<uchar>(i, j) = MAX_GRAY_VALUE - 1;
			else
				dst4.at<uchar>(i, j) = MIN_GRAY_VALUE;
	cv::imshow("img after fuzzy-similarity", dst4);
	imwrite("D:\\result\\test2-fuzzy-similarity.png", dst4);
	
	cv::Mat dst5;
	cv::cvtColor(obj, dst5, CV_RGB2GRAY);
	int thd5 = Fuzzy_Compactness(dst5);
	cout << "fuzzy compactness 阈值：" << thd5 << endl;
	for (i = 0; i < width; i++)
		for (j = 0; j< height; j++)
			if (dst5.at<uchar>(i, j) > thd5)
				dst5.at<uchar>(i, j) = MAX_GRAY_VALUE - 1;
			else
				dst5.at<uchar>(i, j) = MIN_GRAY_VALUE;
	cv::imshow("img after fuzzy-compactness", dst5);
	imwrite("D:\\result\\test2-fuzzy-compactness.png", dst5);
	
	cv::Mat dst6;
	cv::cvtColor(obj, dst6, CV_RGB2GRAY);
	int thd6 = Fuzzy_Huang(dst6);
	cout << "fuzzy Huang's method 阈值：" << thd6 << endl;
	for (i = 0; i < width; i++)
		for (j = 0; j< height; j++)
			if (dst6.at<uchar>(i, j) > thd6)
				dst6.at<uchar>(i, j) = MAX_GRAY_VALUE - 1;
			else
				dst6.at<uchar>(i, j) = MIN_GRAY_VALUE;
	cv::imshow("img after fuzzy-Huang's-method", dst6);
	imwrite("D:\\result\\test2-fuzzy-Huangs-method.png", dst6);

	cv::waitKey(0);
	return 0;
}
*/