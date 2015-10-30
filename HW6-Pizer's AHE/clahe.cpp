#pragma warning(disable:4146)

#include <cv.h>
#include "clahe.h"

#include <stdio.h>
#include <stdlib.h>

#define BYTE_IMAGE
#ifdef BYTE_IMAGE
typedef unsigned char kz_pixel_t;
#define uiNR_OF_GREY (256)
#else
typedef unsigned short kz_pixel_t;
# define uiNR_OF_GREY (4096)
#endif
static int CLAHE(kz_pixel_t* pImage, unsigned int uiXRes,
	unsigned int uiYRes, kz_pixel_t Min,
	kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,
	unsigned int uiNrBins, float fCliplimit);

void cvAdaptEqualize(IplImage *src, IplImage *dst,
	unsigned int xdivs, unsigned int ydivs, unsigned int bins, int range)
{

	// call CLAHE with negative limit (as flag value) to perform AHE (no limits)

	cvCLAdaptEqualize(src, dst, xdivs, ydivs, bins, -1.0, range);
}

void cvCLAdaptEqualize(IplImage *src, IplImage *dst,
	unsigned int xdivs, unsigned int ydivs, unsigned int bins,
	float limit, int range)
{

	double min_val, max_val;
	unsigned char min, max;

	int type;

	if ((src == NULL) || (dst == NULL))
		CV_ERROR(CV_StsUnsupportedFormat, "NULL value passed as image to function");

	CV_CALL(type = cvGetElemType(src));
	if (type != CV_8UC1)
		CV_ERROR(CV_StsUnsupportedFormat, "Only 8uC1 images are supported");
	CV_CALL(type = cvGetElemType(src));
	if (type != CV_8UC1)
		CV_ERROR(CV_StsUnsupportedFormat, "Only 8uC1 images are supported");

	if (src->width != dst->width || src->height != dst->height)
		CV_ERROR(CV_StsUnmatchedSizes, "src and dst images must be equal sizes");

	if (((xdivs < 2) || (xdivs > 16)) || ((ydivs < 2) || (ydivs > 16)))
		CV_ERROR(CV_StsBadFlag, "xdivs and ydivs must in range (MIN=2 MAX = 16)");

	if ((bins < 2) || (bins > 256))
		CV_ERROR(CV_StsBadFlag, "bins must in range (MIN=2 MAX = 256)");

	cvCopy(src, dst);

	int enlarged = 0;
	int origW = dst->width;
	int origH = dst->height;
	IplImage *tmpDst = 0;
	if ((dst->width & (8 - 1)) || (dst->height & (8 - 1)) || (dst->width % xdivs) || (dst->height % ydivs)) {
		int newW = ((dst->width + 8 - 1) & -8);
		newW = ((newW + xdivs - 1) & -xdivs);
		int newH = ((dst->height + ydivs - 1) & -ydivs);

		IplImage *enlargedDst = cvCreateImage(cvSize(newW, newH), dst->depth, dst->nChannels);
		cvResize(dst, enlargedDst, CV_INTER_CUBIC);

		tmpDst = dst;
		dst = enlargedDst;
		enlarged = 1;
	}

	if (dst->width != dst->widthStep)
		CV_ERROR(CV_StsBadFlag, "dst->widthStep should be the same as dst->width. The IplImage has padding, so use a larger image.");

	if (dst->width % xdivs)
		CV_ERROR(CV_StsBadFlag, "xdiv must be an integer multiple of image width ");
	if (dst->height % ydivs)
		CV_ERROR(CV_StsBadFlag, "ydiv must be an integer multiple of image height ");

	if (range == CV_CLAHE_RANGE_INPUT) {
		cvMinMaxLoc(dst, &min_val, &max_val);
		min = (unsigned char)min_val;
		max = (unsigned char)max_val;
	}
	else {
		min = 0;
		max = 255;
	}
	CLAHE((kz_pixel_t*)(dst->imageData), (unsigned int)dst->width, (unsigned int)
		dst->height, (kz_pixel_t)min, (kz_pixel_t)max, (unsigned int)xdivs, (unsigned int)ydivs,
		(unsigned int)bins, (float)limit);

	if (enlarged) {
		cvResize(dst, tmpDst, CV_INTER_CUBIC);	
		cvReleaseImage(&dst);
	}
}
static void ClipHistogram(unsigned long*, unsigned int, unsigned long);
static void MakeHistogram(kz_pixel_t*, unsigned int, unsigned int, unsigned int,
	unsigned long*, unsigned int, kz_pixel_t*);
static void MapHistogram(unsigned long*, kz_pixel_t, kz_pixel_t,
	unsigned int, unsigned long);
static void MakeLut(kz_pixel_t*, kz_pixel_t, kz_pixel_t, unsigned int);
static void Interpolate(kz_pixel_t*, int, unsigned long*, unsigned long*,
	unsigned long*, unsigned long*, unsigned int, unsigned int, kz_pixel_t*);

const static unsigned int uiMAX_REG_X = 16;      /* max. # contextual regions in x-direction */
const static unsigned int uiMAX_REG_Y = 16;      /* max. # contextual regions in y-direction */


static int CLAHE(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes,
	kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,
	unsigned int uiNrBins, float fCliplimit)
{
	unsigned int uiX, uiY;                /* counters */
	unsigned int uiXSize, uiYSize, uiSubX, uiSubY; /* size of context. reg. and subimages */
	unsigned int uiXL, uiXR, uiYU, uiYB;  /* auxiliary variables interpolation routine */
	unsigned long ulClipLimit, ulNrPixels;/* clip limit and region pixel count */
	kz_pixel_t* pImPointer;                /* pointer to image */
	kz_pixel_t aLUT[uiNR_OF_GREY];          /* lookup table used for scaling of input image */
	unsigned long* pulHist, *pulMapArray; /* pointer to histogram and mappings*/
	unsigned long* pulLU, *pulLB, *pulRU, *pulRB; /* auxiliary pointers interpolation */

	if (uiNrX > uiMAX_REG_X) return -1;    /* # of regions x-direction too large */
	if (uiNrY > uiMAX_REG_Y) return -2;    /* # of regions y-direction too large */
	if (uiXRes % uiNrX) return -3;        /* x-resolution no multiple of uiNrX */
	if (uiYRes % uiNrY) return -4;        /* y-resolution no multiple of uiNrY #TPB FIX */
#ifndef BYTE_IMAGE					  /* #TPB FIX */
	if (Max >= uiNR_OF_GREY) return -5;    /* maximum too large */
#endif
	if (Min >= Max) return -6;            /* minimum equal or larger than maximum */
	if (uiNrX < 2 || uiNrY < 2) return -7;/* at least 4 contextual regions required */
	if (fCliplimit == 1.0) return 0;      /* is OK, immediately returns original image. */
	if (uiNrBins == 0) uiNrBins = 128;    /* default value when not specified */

	pulMapArray = (unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);
	if (pulMapArray == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */

	uiXSize = uiXRes / uiNrX; uiYSize = uiYRes / uiNrY;  /* Actual size of contextual regions */
	ulNrPixels = (unsigned long)uiXSize * (unsigned long)uiYSize;

	if (fCliplimit > 0.0) {                /* Calculate actual cliplimit  */
		ulClipLimit = (unsigned long)(fCliplimit * (uiXSize * uiYSize) / uiNrBins);
		ulClipLimit = (ulClipLimit < 1UL) ? 1UL : ulClipLimit;
	}
	else ulClipLimit = 1UL << 14;           /* Large value, do not clip (AHE) */
	MakeLut(aLUT, Min, Max, uiNrBins);    /* Make lookup table for mapping of greyvalues */
	/* Calculate greylevel mappings for each contextual region */
	for (uiY = 0, pImPointer = pImage; uiY < uiNrY; uiY++) {
		for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize) {
			pulHist = &pulMapArray[uiNrBins * (uiY * uiNrX + uiX)];
			MakeHistogram(pImPointer, uiXRes, uiXSize, uiYSize, pulHist, uiNrBins, aLUT);
			ClipHistogram(pulHist, uiNrBins, ulClipLimit);
			MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);
		}
		pImPointer += (uiYSize - 1) * uiXRes;             /* skip lines, set pointer */
	}

	/* Interpolate greylevel mappings to get CLAHE image */
	for (pImPointer = pImage, uiY = 0; uiY <= uiNrY; uiY++) {
		if (uiY == 0) {                                   /* special case: top row */
			uiSubY = uiYSize >> 1;  uiYU = 0; uiYB = 0;
		}
		else {
			if (uiY == uiNrY) {                           /* special case: bottom row */
				uiSubY = uiYSize >> 1;  uiYU = uiNrY - 1;  uiYB = uiYU;
			}
			else {                                        /* default values */
				uiSubY = uiYSize; uiYU = uiY - 1; uiYB = uiYU + 1;
			}
		}
		for (uiX = 0; uiX <= uiNrX; uiX++) {
			if (uiX == 0) {                               /* special case: left column */
				uiSubX = uiXSize >> 1; uiXL = 0; uiXR = 0;
			}
			else {
				if (uiX == uiNrX) {                       /* special case: right column */
					uiSubX = uiXSize >> 1;  uiXL = uiNrX - 1; uiXR = uiXL;
				}
				else {                                    /* default values */
					uiSubX = uiXSize; uiXL = uiX - 1; uiXR = uiXL + 1;
				}
			}

			pulLU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXL)];
			pulRU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXR)];
			pulLB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXL)];
			pulRB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXR)];
			Interpolate(pImPointer, uiXRes, pulLU, pulRU, pulLB, pulRB, uiSubX, uiSubY, aLUT);
			pImPointer += uiSubX;                         /* set pointer on next matrix */
		}
		pImPointer += (uiSubY - 1) * uiXRes;
	}
	free(pulMapArray);                                    /* free space for histograms */
	return 0;                                             /* return status OK */
}
void ClipHistogram(unsigned long* pulHistogram, unsigned int
	uiNrGreylevels, unsigned long ulClipLimit)

{
	unsigned long* pulBinPointer, *pulEndPointer, *pulHisto;
	unsigned long ulNrExcess, ulUpper, ulBinIncr, ulStepSize, i;
	unsigned long ulOldNrExcess;  // #IAC Modification

	long lBinExcess;

	ulNrExcess = 0;  pulBinPointer = pulHistogram;
	for (i = 0; i < uiNrGreylevels; i++) { /* calculate total number of excess pixels */
		lBinExcess = (long)pulBinPointer[i] - (long)ulClipLimit;
		if (lBinExcess > 0) ulNrExcess += lBinExcess;     /* excess in current bin */
	};

	/* Second part: clip histogram and redistribute excess pixels in each bin */
	ulBinIncr = ulNrExcess / uiNrGreylevels;              /* average binincrement */
	ulUpper = ulClipLimit - ulBinIncr;  /* Bins larger than ulUpper set to cliplimit */

	for (i = 0; i < uiNrGreylevels; i++) {
		if (pulHistogram[i] > ulClipLimit) pulHistogram[i] = ulClipLimit; /* clip bin */
		else {
			if (pulHistogram[i] > ulUpper) {              /* high bin count */
				ulNrExcess -= pulHistogram[i] - ulUpper; pulHistogram[i] = ulClipLimit;
			}
			else {                                        /* low bin count */
				ulNrExcess -= ulBinIncr; pulHistogram[i] += ulBinIncr;
			}
		}
	}

	do {   /* Redistribute remaining excess  */
		pulEndPointer = &pulHistogram[uiNrGreylevels]; pulHisto = pulHistogram;

		ulOldNrExcess = ulNrExcess;     /* Store number of excess pixels for test later. */

		while (ulNrExcess && pulHisto < pulEndPointer)
		{
			ulStepSize = uiNrGreylevels / ulNrExcess;
			if (ulStepSize < 1)
				ulStepSize = 1;       /* stepsize at least 1 */
			for (pulBinPointer = pulHisto; pulBinPointer < pulEndPointer && ulNrExcess;
				pulBinPointer += ulStepSize)
			{
				if (*pulBinPointer < ulClipLimit)
				{
					(*pulBinPointer)++;  ulNrExcess--;    /* reduce excess */
				}
			}
			pulHisto++;       /* restart redistributing on other bin location */
		}
	} while ((ulNrExcess) && (ulNrExcess < ulOldNrExcess));
	/* Finish loop when we have no more pixels or we can't redistribute any more pixels */


}

void MakeHistogram(kz_pixel_t* pImage, unsigned int uiXRes,
	unsigned int uiSizeX, unsigned int uiSizeY,
	unsigned long* pulHistogram,
	unsigned int uiNrGreylevels, kz_pixel_t* pLookupTable)
{
	kz_pixel_t* pImagePointer;
	unsigned int i;

	for (i = 0; i < uiNrGreylevels; i++) pulHistogram[i] = 0L; /* clear histogram */

	for (i = 0; i < uiSizeY; i++) {
		pImagePointer = &pImage[uiSizeX];
		while (pImage < pImagePointer) pulHistogram[pLookupTable[*pImage++]]++;
		pImagePointer += uiXRes;
		pImage = &pImagePointer[-uiSizeX];
	}
}

void MapHistogram(unsigned long* pulHistogram, kz_pixel_t Min, kz_pixel_t Max,
	unsigned int uiNrGreylevels, unsigned long ulNrOfPixels)
{
	unsigned int i;  unsigned long ulSum = 0;
	const float fScale = ((float)(Max - Min)) / ulNrOfPixels;
	const unsigned long ulMin = (unsigned long)Min;

	for (i = 0; i < uiNrGreylevels; i++) {
		ulSum += pulHistogram[i]; pulHistogram[i] = (unsigned long)(ulMin + ulSum*fScale);
		if (pulHistogram[i] > Max) pulHistogram[i] = Max;
	}
}

void MakeLut(kz_pixel_t * pLUT, kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrBins)

{
	int i;
	const kz_pixel_t BinSize = (kz_pixel_t)(1 + (Max - Min) / uiNrBins);

	for (i = Min; i <= Max; i++)  pLUT[i] = (i - Min) / BinSize;
}

void Interpolate(kz_pixel_t * pImage, int uiXRes, unsigned long * pulMapLU,
	unsigned long * pulMapRU, unsigned long * pulMapLB, unsigned long * pulMapRB,
	unsigned int uiXSize, unsigned int uiYSize, kz_pixel_t * pLUT)
{
	const unsigned int uiIncr = uiXRes - uiXSize; /* Pointer increment after processing row */
	kz_pixel_t GreyValue; unsigned int uiNum = uiXSize*uiYSize; /* Normalization factor */

	unsigned int uiXCoef, uiYCoef, uiXInvCoef, uiYInvCoef, uiShift = 0;

	if (uiNum & (uiNum - 1))   /* If uiNum is not a power of two, use division */
		for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;
			uiYCoef++, uiYInvCoef--, pImage += uiIncr) {
		for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;
			uiXCoef++, uiXInvCoef--) {
			GreyValue = pLUT[*pImage];             /* get histogram bin value */
			*pImage++ = (kz_pixel_t)((uiYInvCoef * (uiXInvCoef*pulMapLU[GreyValue]
				+ uiXCoef * pulMapRU[GreyValue])
				+ uiYCoef * (uiXInvCoef * pulMapLB[GreyValue]
				+ uiXCoef * pulMapRB[GreyValue])) / uiNum);
		}
	}
	else {                         /* avoid the division and use a right shift instead */
		while (uiNum >>= 1) uiShift++;             /* Calculate 2log of uiNum */
		for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;
			uiYCoef++, uiYInvCoef--, pImage += uiIncr) {
			for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;
				uiXCoef++, uiXInvCoef--) {
				GreyValue = pLUT[*pImage];         /* get histogram bin value */
				*pImage++ = (kz_pixel_t)((uiYInvCoef* (uiXInvCoef * pulMapLU[GreyValue]
					+ uiXCoef * pulMapRU[GreyValue])
					+ uiYCoef * (uiXInvCoef * pulMapLB[GreyValue]
					+ uiXCoef * pulMapRB[GreyValue])) >> uiShift);
			}
		}
	}
}
