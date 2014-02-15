#ifndef __CV_SET_CARD_H
#define __CV_SET_CARD_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <math.h>

#include "cvSetUtils.h"

using namespace cv;
using namespace std;


class SetCard {
	public:
        SetCard(Mat &img, cvOptions *options);
		void Prosses(void);
		void DisplayOnImage(void);
		Mat & getMat(void);
        CardProperties mCardProperties;
        bool mFound;
        vector<vector<Point> > mShapeContour;


	protected:
        void NormalizeColors(void);
		void CutImage(void);
		void DetectAmount(void);
		void DetectShading(void);
		void DetectSymbol(void);
        void DetectColor(void);

		double *mContoursArea;
		vector<Vec4i> 		   mHierarchy;
        vector<Point> mContour;
        vector<vector<Point> > mContours;
        cvOptions *mOptions;

	private:
		void LoadImage(char *imgPath);
		void ShowImageDebug(Mat I, char *c);

		Mat mImg;
		Mat mImgGray;
		Mat mImgBW;
		Mat mImgMask;
        Mat mImgNormColors;

        Rect mMaskRect;

		Mat mCutImg;
		Mat mCutBW;
		Mat mCutMask;
        Mat mCutNormColors;

};

#endif
