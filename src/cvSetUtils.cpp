#include "cvSetUtils.h"

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

static int gNUM = 0;

const string e2sSymbols[]  = {"oval", "squiggle", "diamond"};
const string e2sColors[]   = {"green", "purple", "red"};
const string e2sAmounts[]  = {"1", "2", "3"};
const string e2sShadings[] = {"solid", "open", "striped"};


const Scalar COLORS_LIST[] = { Scalar(  0,   0, 255),
                               Scalar(  0, 255,   0),
                               Scalar(255,   0,   0),
                               Scalar(  0, 255, 255),
                               Scalar(255, 255,   0),
                               Scalar(255,   0, 255)
};


struct IndexValue{
    float value;
    int index;
    int idxIndex;
};

static inline float getMean(std::vector<int> &idx, double a[])
{
    float r = 0;
    for(int i=0; i<idx.size();i++)
        r+= a[idx.at(i)];
    return r/idx.size();
}

static inline IndexValue findClosestPoint(std::vector<int> &idx, double a[], float p)
{
    IndexValue r;
    r.index = -1;
    r.value = -1;
    for(int i=0; i<idx.size();i++)
    {
        float f = abs(a[idx.at(i)]-p);
        if ((f < r.value) || (r.value < 0))
        {
            r.value = f;
            r.index = idx.at(i);
            r.idxIndex = i;
        }
    }
    return r;
}

static inline IndexValue findFarestPoint(std::vector<int> &idx, double a[])
{
    IndexValue r;
    r.index = -1;
    r.value = -1;
    float m = getMean(idx, a);
    for(int i=0; i<idx.size();i++)
    {
        float f = abs(a[idx.at(i)]-m);
        if ((f > r.value) || (r.value < 0))
        {
            r.value = f;
            r.index = idx.at(i);
            r.idxIndex = i;
        }
    }
    return r;
}


int* find12Contours(double a[], int s, int minsSize)
{
    int * idxArray = new int[12];
    vector<int> allIndex;
    vector<int> selIndex;

    for (int i=0;i<s;i++)
        if (a[i]>minsSize)
            allIndex.push_back(i);

    if (allIndex.size()<12)
        return NULL;

    selIndex.push_back(allIndex.back());
    allIndex.pop_back();

    while (true)
    {
        float m = getMean(selIndex, a);
        IndexValue imin = findClosestPoint(allIndex, a, m);
        if (selIndex.size() == 12)
        {
            IndexValue imax = findFarestPoint(selIndex, a);
            if (imax.value <= imin.value)
                break;
            else
            {
                selIndex.erase(selIndex.begin() + imax.idxIndex);
                allIndex.push_back(imax.index);
            }
        }
        selIndex.push_back(imin.index);
        allIndex.erase(allIndex.begin() + imin.idxIndex);
    }
    for (int i=0;i<selIndex.size();i++)
    {
        idxArray[i] = selIndex.at(i);
    }
    return idxArray;
}


void clustering(Mat &src, Mat &d)
{
    Mat samples(src.rows * src.cols, 3, CV_32F);
      for( int y = 0; y < src.rows; y++ )
        for( int x = 0; x < src.cols; x++ )
          for( int z = 0; z < 3; z++)
              samples.at<float>(y + x*src.rows, z) = src.at<Vec3b>(y,x)[z];


      int clusterCount = 3;
      Mat labels;
      int attempts = 1;
      Mat centers;
      kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );


      Mat new_image( src.size(), src.type() );
      for( int y = 0; y < src.rows; y++ )
        for( int x = 0; x < src.cols; x++ )
        {
          int cluster_idx = labels.at<int>(y + x*src.rows,0);
          new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
          new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
          new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
        }
      new_image.copyTo(d);
}


void show(Mat &m)
{
    gNUM++;
    char b[255];
    sprintf(b, "Image %d", gNUM);
    namedWindow(b, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
    imshow(b , m );
}



int getThresholdVal(Mat &m)
{
    int histSize = 255;
    int *histVals = showImgHist(m, histSize, false);
    int thresholdVal = -1;
    int r = 115;

    for (int i=0;i<histSize;i++)
        if (thresholdVal < histVals[i]){
            thresholdVal = histVals[i];
            r = i;
        }

    return r;
}


int *showImgHist(Mat &m, int histSize, bool showHist)
{

    char b[255];

    int histMaxHight = 512;
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;

    int *histVal = new int[histSize];

    Mat hist;
    calcHist( &m, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );
    //normalize(hist, hist, 0, histMaxHight, NORM_MINMAX, -1, Mat() );

    for( int i = 1; i < histSize; i++ )
        histVal[i] = cvRound(hist.at<float>(i));

    if (showHist)
    {
        int hist_w = 512; int hist_h = histMaxHight;
        int bin_w = cvRound( (double) hist_w/histSize );

        Mat histImage( hist_h, hist_w, CV_8UC1, Scalar(0) );


        for( int i = 1; i < histSize; i++ )
            line( histImage, Point( bin_w*(i-1), hist_h - histVal[i-1]),
                    Point( bin_w*(i), hist_h - histVal[i]),
                    Scalar( 255), 2, 8, 0  );

        gNUM++;
        sprintf(b, "Image Hist %d", gNUM);
        namedWindow(b, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
        imshow(b , histImage );
    }

}


void tplay(Mat &m)
{
    Mat HSV;

    cvtColor(m, HSV, COLOR_BGR2HSV);
    vector<Mat> CHNL(3);
    split(HSV, CHNL);

    showImgHist(CHNL[1], 255, true);

    /*
    threshold(CHNL[1], CHNL[1],
              50, 255, THRESH_BINARY);
    Mat I;
    merge(CHNL.data(), 3, I);
    show(I);
    */
    Mat D;
    clustering(m, D);
    show(D);

}
