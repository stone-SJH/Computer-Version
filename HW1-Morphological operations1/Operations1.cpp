#include "opencv.h"

#define TYPE_MORPH_RECT      (0)
#define TYPE_MORPH_CROSS     (1)
#define TYPE_MORPH_ELLIPSE   (2)

#define MAX_ELE_TYPE         (2)
#define MAX_ELE_SIZE         (20)

Mat src, erode_dst, dilate_dst, open_dst, close_dst;

const char *erode_wn = "eroding demo";
const char *dilate_wn = "dilating demo";
const char *open_wn = "openning demo";
const char *close_wn = "closing demo";

int erode_ele_type;
int dilate_ele_type;
int open_ele_type;
int close_ele_type;
int erode_ele_size;
int dilate_ele_size;
int open_ele_size;
int close_ele_size;

static void Erosion(int, void *);
static void Dilation(int, void *);
static void Openning(int, void *);
static void Closing(int, void *);

int main()
{
	src = imread("D:\\pictures\\test2.png");
	if (!src.data) {
		cout << "Read image failure." << endl;
		return -1;
	}

	
	
	

	//Erosion
	namedWindow(erode_wn, WINDOW_AUTOSIZE);
	createTrackbar("Element Type\n0:Rect\n1:Cross\n2:Ellipse", erode_wn,
		&erode_ele_type, MAX_ELE_TYPE, Erosion);
	createTrackbar("Element Size: 2n+1", erode_wn,
		&erode_ele_size, MAX_ELE_SIZE, Erosion);
	Erosion(0, 0);

	//Dilation
	namedWindow(dilate_wn, WINDOW_AUTOSIZE);
	createTrackbar("Element Type\n0:Rect\n1:Cross\n2:Ellipse", dilate_wn,
		&dilate_ele_type, MAX_ELE_TYPE, Dilation);
	createTrackbar("Element Size: 2n+1", dilate_wn,
		&dilate_ele_size, MAX_ELE_SIZE, Dilation);
	Dilation(0, 0);

	//Openning
	namedWindow(open_wn, WINDOW_AUTOSIZE);
	createTrackbar("Element Type\n0:Rect\n1:Cross\n2:Ellipse", open_wn,
		&open_ele_type, MAX_ELE_TYPE, Openning);
	createTrackbar("Element Size: 2n+1", open_wn,
		&open_ele_size, MAX_ELE_SIZE, Openning);
	Openning(0, 0);

	//Closing
	namedWindow(close_wn, WINDOW_AUTOSIZE);
	createTrackbar("Element Type\n0:Rect\n1:Cross\n2:Ellipse", close_wn,
		&close_ele_type, MAX_ELE_TYPE, Closing);
	createTrackbar("Element Size: 2n+1", close_wn,
		&close_ele_size, MAX_ELE_SIZE, Closing);
	Closing(0, 0);


	waitKey(0);

	return 0;
}

static void Erosion(int, void *)
{
	int erode_type;

	switch (erode_ele_type) {
	case TYPE_MORPH_RECT:
		erode_type = MORPH_RECT;
		break;
	case TYPE_MORPH_CROSS:
		erode_type = MORPH_CROSS;
		break;
	case TYPE_MORPH_ELLIPSE:
		erode_type = MORPH_ELLIPSE;
		break;
	default:
		erode_type = MORPH_RECT;
		break;
	}

	Mat ele = getStructuringElement(erode_type, Size(2 * erode_ele_size + 1, 2 * erode_ele_size + 1),
		Point(erode_ele_size, erode_ele_size));

	erode(src, erode_dst, ele);

	imshow(erode_wn, erode_dst);
}

static void Dilation(int, void *)
{
	int dilate_type;

	switch (dilate_ele_type) {
	case TYPE_MORPH_RECT:
		dilate_type = MORPH_RECT;
		break;
	case TYPE_MORPH_CROSS:
		dilate_type = MORPH_CROSS;
		break;
	case TYPE_MORPH_ELLIPSE:
		dilate_type = MORPH_ELLIPSE;
		break;
	default:
		dilate_type = MORPH_RECT;
		break;
	}

	Mat ele = getStructuringElement(dilate_type, Size(2 * dilate_ele_size + 1, 2 * dilate_ele_size + 1),
		Point(dilate_ele_size, dilate_ele_size));

	dilate(src, dilate_dst, ele);

	imshow(dilate_wn, dilate_dst);
}

static void Openning(int, void *)
{
	int open_type;

	switch (open_ele_type) {
	case TYPE_MORPH_RECT:
		open_type= MORPH_RECT;
		break;
	case TYPE_MORPH_CROSS:
		open_type = MORPH_CROSS;
		break;
	case TYPE_MORPH_ELLIPSE:
		open_type = MORPH_ELLIPSE;
		break;
	default:
		open_type = MORPH_RECT;
		break;
	}

	Mat ele = getStructuringElement(open_type, Size(2 * open_ele_size + 1, 2 * open_ele_size + 1),
		Point(open_ele_size, open_ele_size));

	erode(src, open_dst, ele);
	dilate(open_dst, open_dst, ele);

	imshow(open_wn, open_dst);
}

static void Closing(int, void *)
{
	int close_type;

	switch (close_ele_type) {
	case TYPE_MORPH_RECT:
		close_type = MORPH_RECT;
		break;
	case TYPE_MORPH_CROSS:
		close_type = MORPH_CROSS;
		break;
	case TYPE_MORPH_ELLIPSE:
		close_type = MORPH_ELLIPSE;
		break;
	default:
		close_type = MORPH_RECT;
		break;
	}

	Mat ele = getStructuringElement(close_type, Size(2 * close_ele_size + 1, 2 * close_ele_size + 1),
		Point(close_ele_size, close_ele_size));

	dilate(src, close_dst, ele);
	erode(close_dst, close_dst, ele);

	imshow(close_wn, close_dst);
}