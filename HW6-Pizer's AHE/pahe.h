#include <cv.h>

#define CV_CLAHE_RANGE_FULL 0
#define CV_CLAHE_RANGE_INPUT 1

void cvAdaptEqualize(IplImage *src, IplImage *dst,
	unsigned int xdivs, unsigned int ydivs, unsigned int bins, int range);

void cvCLAdaptEqualize(IplImage *src, IplImage *dst,
	unsigned int xdivs, unsigned int ydivs, unsigned int bins, float limit,
	int range);

#undef CV_ERROR
#define CV_ERROR( Code, Msg )                                       \
{                                                                   \
     cvError( (Code), "CLAHE code", Msg, __FILE__, __LINE__ );       \
     exit(1);                                                          \
}
