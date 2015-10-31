#include "opencv.h"

void GetSkeleton(uchar* imagedata, int width, int height){
	long length;
	uchar p0, p1, p2, p3, p4, p5, p6, p7;
	uchar *pos, *postemp;
	uchar sum;
	bool thined = true;
	length = width * height;
	uchar *pTemp = new uchar[sizeof(uchar) * width * height]();

	while (thined){
		thined = false;

		pos = (uchar *)imagedata + width + 1;
		memset(pTemp, 0, length);
		postemp = (uchar *)pTemp + width + 1;

		//check every pixel
		//find *:
		//   + + +
		//   + * +
		//   + + +
		//that means when all the + is 1, we dont need to thin this point this time.
		//o.w. we need to thin it.
		for (int i = 1; i < height - 1; i++){
			for (int j = 1; j < width - 1; j++){
				if (*pos == 0){
					pos++;
					postemp++;
					continue;
				}

				p3 = *(pos + 1);
				p2 = *(pos + 1 - width);
				p1 = *(pos - width);
				p0 = *(pos - width - 1);
				p7 = *(pos - 1);
				p6 = *(pos + width - 1);
				p5 = *(pos + width);
				p4 = *(pos + width + 1);
				sum = p0 & p1 & p2 & p3 & p4 & p5 & p6 & p7;
				if (sum == 0){
					*postemp = 1;                     
				}

				pos++;
				postemp++;
			}
			pos++;
			pos++;
			postemp++;
			postemp++;
		}
		//now we get the table of pixels that need to be thin
		//then we cal the value of p0*2^0+p1*2^1+...+p7*2^7, and find it in the index table
		//here p0->p7:
		//p0 p1 p2
		//p7 *  p3
		//p6 p5 p4
		//if we get it 1 in index table, then delete this pixel
		//and then begin another round.
		pos = (uchar *)imagedata + width + 1;
		postemp = (uchar *)pTemp + width + 1;
		for (int i = 1; i < height - 1; i++){
			for (int j = 1; j < width - 1; j++){
				if (*postemp == 0){
					pos++;
					postemp++;
					continue;
				}

				p3 = *(pos + 1);
				p2 = *(pos - width + 1);
				p1 = *(pos - width);
				p0 = *(pos - width - 1);
				p7 = *(pos - 1);
				p6 = *(pos + width - 1);
				p5 = *(pos + width);
				p4 = *(pos + width + 1);

				p1 *= 2;
				p2 *= 4;
				p3 *= 8;
				p4 *= 16;
				p5 *= 32;
				p6 *= 64;
				p7 *= 128;

				sum = p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7;
				if (deletemark[sum] == 1){
					*pos = 0;
					thined = true;
				}
				pos++;
				postemp++;
			}

			pos++;
			pos++;
			postemp++;
			postemp++;
		}
	}
	delete[]pTemp;
}

int main()
{
	Mat src = imread("D:\\pictures\\potato.png");
	imshow("origin", src);
	Mat dst = Otsu(src);

	int height = dst.cols;
	int width = dst.rows;
	uchar* imagedata = new uchar[height * width]();
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			imagedata[i * height + j] = dst.at<uchar>(i, j) > 0 ? 1 : 0;
		}
	}
	GetSkeleton(imagedata, height, width);
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			dst.at<uchar>(i, j) = imagedata[i * height + j] > 0 ? 0 : 255;
		}
	}


	cvtColor(src, src, CV_RGB2GRAY);
	Mat sdst, dst1, dst2;
	dst1 = Boundary(src);
	Mat BigBlankimage = Mat::ones(src.rows, src.cols, CV_8UC1);
	dst2 = BigBlankimage * 255 - dst1;
	int threhold = otsu(dst1) - 28;
	threshold(dst1, sdst, threhold, 255, CV_THRESH_BINARY);
	sdst = BigBlankimage * 255 - sdst;

	imshow("skin", sdst);
	imshow("skeleton", dst);

	waitKey();

}
