#ifndef CARDPROPERTIES_H
#define CARDPROPERTIES_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace std;
using namespace cv;


extern const Scalar COLORS_LIST[];
extern const string e2sSymbols[];
extern const string e2sColors[];
extern const string e2sAmounts[];
extern const string e2sShadings[];

enum CARD_SYMBOLS {
    CARD_SYMBOL_OVAL = 0,
    CARD_SYMBOL_SQUIGGLE = 1,
    CARD_SYMBOL_DIAMOND = 2
};

enum CARD_COLORS {
    CARD_COLOR_GREEN = 0,
    CARD_COLOR_PURPLE = 1,
    CARD_COLOR_RED = 2
};

enum CARD_AMOUNTS {
    CARD_AMOUNT_ONE = 0,
    CARD_AMOUNT_TWO = 1,
    CARD_AMOUNT_THREE = 2
};

enum CARD_SHADINGS {
    CARD_SHADING_SOLID = 0,
    CARD_SHADING_OPEN = 1,
    CARD_SHADING_STRIPED = 2
};

enum DEBUG_LEVELS{
    DEBUG_NONE = 0,
    DEBUG_SHOW_LINUX = 0x01,
    DEBUG_SHOW_ALL = 0xFF
};

struct cvOptions {
    static cvOptions *getInstance();
    int mThresholdVal = 115;
    int mThresholdCardsVal = 115;

    int mColor2Gray   = CV_BGR2GRAY;

    bool mColor2NormalizeNeeded = false;
    int mColor2Normalize = COLOR_RGBA2RGB;

    int mColor2HSV = COLOR_BGR2HSV;

    float mShadingLow  = 0.03;
    float mShadingHigh = 0.1;

    DEBUG_LEVELS DEBUG_LEVEL;
};

struct CardProperties
{
    //CardProperties();
    CARD_SYMBOLS  mSymbol;
    CARD_COLORS   mColor;
    CARD_AMOUNTS  mAmount;
    CARD_SHADINGS mShading;
    float mShadingProp;
};


int* find12Contours(double a[], int s, int minsSize = 10);
void clustering(Mat &src, Mat &d);
void show(Mat &m);
int *showImgHist(Mat &m, int histSize=255, bool showHist=false);
int getThresholdVal(Mat &m);
void tplay(Mat &m);


#endif // CARDPROPERTIES_H
