/*
 * cvSetGameBoard.h
 *
 *  Created on: Jan 28, 2014
 *      Author: amit
 */

#ifndef CVSETGAMEBOARD_H_
#define CVSETGAMEBOARD_H_


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <math.h>

#include "cvSetUtils.h"
#include "cvSetCard.h"

using namespace cv;
using namespace std;


class SetGameBoard {
public:
    SetGameBoard(Mat &img, cvOptions *);
	virtual ~SetGameBoard();

	void Prosses(void);
	void SearchForCards(void);
	void ProssesCards(void);
	void SolveGame();

protected:
	vector<vector<Point> > mContours;
	double *mContoursArea;
	vector<Vec4i> 		   mHierarchy;
	vector<Rect> mCardsRect;
    vector<vector<Point> > mCardsContours;
	vector<SetCard> mSetCards;
	Rect mRect;
	bool mFoundAllCards;
    cvOptions *mOptions;

private:
	Mat mImg;
};

#endif /* CVSETGAMEBOARD_H_ */
