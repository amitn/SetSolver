#include <iostream>
#include <stdio.h>

#include "cvSetCard.h"
#include "cvSetUtils.h"

inline int CountNonZeroInRage(Mat & M, int l, int h)
{
    Mat t;
    inRange(M, l, h, t);
    return countNonZero(t);
}

SetCard::SetCard(Mat &img, cvOptions *options)
{
    mImg = img;
    mOptions = options;

}


void SetCard::Prosses(void)
{
    NormalizeColors();
    DetectAmount();
    mFound = (mContours.size() > 0);
    if (mFound)
    {
    	CutImage();
    	DetectShading();
    	DetectSymbol();
    	DetectColor();
    }

}

void SetCard::ShowImageDebug(Mat I, char *c)
{
    if (mOptions->DEBUG_LEVEL && DEBUG_SHOW_LINUX)
		imshow(c, I);
}


void SetCard::DetectAmount(void)
{
    show(mImg);
    tplay(mImgNormColors);

    //cvtColor(mImgNormColors, mImgGray, mOptions->mColor2Gray);
    cvtColor(mImg, mImgGray, mOptions->mColor2Gray);

    //equalizeHist(mImgGray, TT);

    show(mImgGray);
    //showImgHist(mImgGray, 255, true);

    threshold(mImgGray, mImgBW,
              getThresholdVal(mImgGray), 255, THRESH_BINARY_INV);
	

    show(mImgBW);
    //bitwise_not(mImgBW, mImgBW);

    Mat tBW = mImgBW.clone();
    findContours( tBW, mContours, mHierarchy,
			RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    mContoursArea = new double[mContours.size()];
	int v = 0;
	float areaAvg = 0;
	int minArea = mImg.cols * mImg.rows * 0.0025;
    for( size_t k = 0; k < mContours.size(); k++ )
	{
        mContoursArea[k] = contourArea(mContours[k]);
        if (mContoursArea[k] > minArea) {
			v++;
            areaAvg += mContoursArea[k];
		}
	}
	areaAvg = (areaAvg/v)*0.8;

	bool wasMasked = false;
    Mat tContours = Mat::zeros(mImgBW.size(), CV_8U);
    mImgMask = Mat::zeros(mImgBW.size(), CV_8U);
    for( size_t k = 0; k < mContours.size(); k++ )
	{
        if (mContoursArea[k] >= areaAvg)
		{
            drawContours(tContours, mContours, k, 255, -1);
			if (!wasMasked)
			{
				wasMasked = true;
                drawContours(mImgMask, mContours, k, 255, -1);
                mMaskRect = boundingRect(mContours[k]);
                mContour = mContours[k];
                mShapeContour.push_back(mContours[k]);
			}
		}
	}


    findContours( tContours, mContours, mHierarchy,
			RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    mCardProperties.mAmount = (CARD_AMOUNTS)(mContours.size()-1);
}

void SetCard::NormalizeColors(void)
{
	Mat tImg;
    Mat iNormColors;
    Mat iNormColorsTmp;
    Mat iNorm;
    int cols, rows;
    if (mOptions->mColor2NormalizeNeeded)
        cvtColor(mImg, tImg, mOptions->mColor2Normalize);
    else
        tImg = mImg;
    tImg.convertTo(iNormColors, CV_32F);
    tImg.convertTo(iNormColorsTmp, CV_32F);
    cols = iNormColorsTmp.cols;
    rows = iNormColorsTmp.rows;
    pow(iNormColorsTmp, 2, iNormColorsTmp);
    iNormColorsTmp = iNormColorsTmp.reshape(1,  (cols * rows) );
    reduce(iNormColorsTmp, iNorm, 1, CV_REDUCE_SUM);
    iNorm = iNorm.reshape(1, rows);
    sqrt(iNorm, iNorm);
    Mat channels[] = {iNorm, iNorm, iNorm};
    merge(channels, 3 , iNorm);

    divide(iNormColors, iNorm, mImgNormColors, CV_32FC3);
    normalize(mImgNormColors, mImgNormColors, 0, 255,
            NORM_MINMAX, CV_8UC3);
}

void SetCard::CutImage(void)
{
    Rect r = mMaskRect;
	r -= Point(5, 5);
	r += Size(10, 10);
	r.x = (r.x < 0) ? 0 : r.x;
	r.y = (r.y < 0) ? 0 : r.y;
	r.height = (r.height + r.y > mImg.rows) ? mImg.rows - r.y : r.height;
	r.width =  (r.width  + r.x > mImg.cols) ? mImg.cols - r.x : r.width;
    mCutImg = mImg(r).clone();
    mCutBW = mImgBW(r).clone();
    mCutMask = mImgMask(r).clone();
    mCutNormColors = mImgNormColors(r).clone();
}

void SetCard::DetectSymbol(void)
{
    Moments mmnts = moments(mCutMask, true);
	double hu[7];
   	HuMoments(mmnts, hu);
	if (hu[0] < 0.207){
        mCardProperties.mSymbol = CARD_SYMBOL_OVAL;
	} else if (hu[0] > 0.23){
        mCardProperties.mSymbol = CARD_SYMBOL_SQUIGGLE;
	} else {
        mCardProperties.mSymbol = CARD_SYMBOL_DIAMOND;
	}
}

void SetCard::DetectShading(void)
{
	Mat S;
	Mat E;
    bitwise_and(mCutBW, mCutMask, S);
	Canny(S, E, 90, 200, 3);
	int nzE = countNonZero(E);
    int nzM = countNonZero(mCutMask);
    //show(mCutBW);
    //show(mCutMask);
    //show(S);
    //show(E);
	float dEM = float(nzE)/float(nzM);
    if (dEM < mOptions->mShadingLow){
        mCardProperties.mShading = CARD_SHADING_SOLID;
    } else if (dEM > mOptions->mShadingHigh){
        mCardProperties.mShading = CARD_SHADING_STRIPED;
	} else {
        mCardProperties.mShading = CARD_SHADING_OPEN;
	}

    mCardProperties.mShadingProp = dEM;

}

void SetCard::DetectColor(void)
{
    Mat M;
    Mat C;
    Mat HSV;
    bitwise_and(mCutBW, mCutMask, M);
    //bitwise_and(mCutImg, mCutImg, C, M);
    bitwise_and(mCutNormColors, mCutNormColors, C, M);

    cvtColor(C, HSV, mOptions->mColor2HSV);
    vector<Mat> CHNL(3);
    split(HSV, CHNL);

    vector<int> nGPR(3);
    nGPR[0] = CountNonZeroInRage(CHNL[0],  25, 90);  // green
    nGPR[1] = CountNonZeroInRage(CHNL[0], 140, 170); // purple
    nGPR[2] = CountNonZeroInRage(CHNL[0], 170, 255); // red
    double maxV;
    int i;
    minMaxIdx(Mat(nGPR), NULL, &maxV, NULL, &i);
    mCardProperties.mColor = (CARD_COLORS)i;
}

Mat & SetCard::getMat(void)
{
	return mImg;
}
