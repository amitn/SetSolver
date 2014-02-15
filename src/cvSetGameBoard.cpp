#include "cvSetGameBoard.h"
#include <stdio.h>


static const char *CARDS_FOUND_ALL    = "Found 12 Cards";
static const char *CARDS_MISSING_SOME = "Not All Cards Found !!!";

static void moveContour(vector<Point>& contour, int dx, int dy)
{
    for (size_t i=0; i<contour.size(); i++)
    {
        contour[i].x += dx;
        contour[i].y += dy;
    }
}

SetGameBoard::SetGameBoard(Mat &img, cvOptions *options) {
	mImg = img;
    mOptions = options;
}

SetGameBoard::~SetGameBoard() {
}


void SetGameBoard::Prosses(void){
	SearchForCards();
	if (mFoundAllCards)
	{
		ProssesCards();
	}
}

void SetGameBoard::SearchForCards(void)
{
	Mat iGray;
	Mat iBW;
    cvtColor(mImg, iGray, mOptions->mColor2Gray);
	threshold(iGray, iBW,
                mOptions->mThresholdCardsVal, 255, THRESH_BINARY);

    show(iBW);
	Mat tBW = iBW.clone();

    findContours( tBW, mContours, mHierarchy,
			RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    mContoursArea = new double[mContours.size()];

    for( size_t k = 0; k < mContours.size(); k++ )
        mContoursArea[k] = contourArea(mContours[k]);
    int minSize = 10;//iBW.cols*iBW.rows*0.1*0.1;
    int *idx = find12Contours(mContoursArea, mContours.size(), minSize);
    mFoundAllCards = (idx != NULL);
    if (mFoundAllCards)
    {
        for( size_t k = 0; k < 12; k++ )
        {
            size_t kk = idx[k];
            mRect = boundingRect(mContours[kk]);
            mCardsRect.push_back(mRect);
            mCardsContours.push_back(mContours[kk]);
            rectangle(mImg, mRect, COLORS_LIST[3], 3);
        }
    }

	putText(mImg,  mFoundAllCards ? CARDS_FOUND_ALL : CARDS_MISSING_SOME,
            Point(50,50),
            CV_FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 1);
    show(mImg);
}

void SetGameBoard::ProssesCards(void)
{
    for (int k=0; k < mCardsRect.size(); k++)
    {
        Rect r = mCardsRect.at(k);
        Size sSize = mImg(r).size();
        Mat cImg(sSize, CV_8UC3, Scalar(255, 255, 255));
        //Mat cImg(sSize, CV_8UC3, Scalar(0, 0, 0));
        Mat tMask(sSize, CV_8U, Scalar(0));

        moveContour(mCardsContours[k], -r.x, -r.y);
        drawContours(tMask, mCardsContours, k, Scalar(255), -1);
        mImg(r).copyTo(cImg, tMask);
        //show(cImg);
        SetCard card = SetCard(cImg, mOptions);
        mSetCards.push_back(card);
        card.Prosses();
        int h = r.height / 4;
        int y = r.y + r.height / 8;
        waitKey(0);
        destroyAllWindows();
        if (card.mFound)
        {
            int x = r.x + r.width*0.1;
            char b[255];
            moveContour(card.mShapeContour[0], r.x, r.y);
            drawContours(mImg, card.mShapeContour, 0, COLORS_LIST[1], 1);

/*
            putText(mImg, e2sAmounts[(int)card.mCardProperties.mAmount],
                    Point(x, y ),
                    CV_FONT_HERSHEY_SIMPLEX, 0.4, COLORS_LIST[0], 1);

            putText(mImg, e2sColors[(int)card.mCardProperties.mColor],
                    Point(x, y + h),
                    CV_FONT_HERSHEY_SIMPLEX, 0.4, COLORS_LIST[0], 1);

            putText(mImg, e2sShadings[(int)card.mCardProperties.mShading],
                    Point(x, y + h + h),
                    CV_FONT_HERSHEY_SIMPLEX, 0.4, COLORS_LIST[0], 1);

            sprintf(b, "<%01.4f<%01.4f [%01.4f]", mOptions->mShadingLow,
                    mOptions->mShadingHigh,
                    card.mCardProperties.mShadingProp);

            putText(mImg, b,
                    Point(x + 50, y + h + h),
                    CV_FONT_HERSHEY_SIMPLEX, 0.4, COLORS_LIST[0], 1);

            putText(mImg, e2sSymbols[(int)card.mCardProperties.mSymbol],
                    Point(x, y + h + h + h),
                    CV_FONT_HERSHEY_SIMPLEX, 0.4, COLORS_LIST[0], 1);
*/
        }

    }


}
