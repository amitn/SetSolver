#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>

#include "cvSetCard.h"
#include "cvSetGameBoard.h"
#include "cvSetUtils.h"

using namespace cv;
using namespace std;


int main( int argc, char** argv )
{
	if (argc != 2)
	{
		printf("Please provide a game image\n");
		return -1;
	}
    cvOptions opt;
    opt.mThresholdVal = 150;
    opt.mThresholdCardsVal = 115;
    opt.mColor2Normalize = cv::COLOR_BGR2GRAY;
    opt.mColor2Gray = COLOR_BGR2GRAY;
    opt.mColor2HSV = COLOR_BGR2HSV_FULL;
        
	Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	if (! img.data)
	{
		printf("Could not open the image %s\n", argv[1]);
		return -1;
	}

    show(img);
    SetGameBoard b = SetGameBoard(img, &opt);
    b.Prosses();

    waitKey(0);

    destroyAllWindows();
    
	return 0;
}
