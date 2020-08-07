#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

void BrightnessAndContrastAuto(const cv::Mat &src, cv::Mat &dst, float clipHistPercent );

/** @function main */
int top, bottom;
// skc : (1298,966)   
// car (redraw) : (1278,958) parameter used in moildev : (1320, 1017)
// yu :  (1226,966)
int main(int argc, char** argv)
{
  Mat src, src_gray;
  Mat dst;
  double ratio;

  /// Read the image
  src = imread( argv[1], 1 );
  if( !src.data )
    { return -1; }

ratio = 1.0 ; 
if ( src.cols > 640 ) {
	ratio = src.cols / 640 ; 
	int h = (int)( src.rows / ratio ); 
	resize(src, src, Size(640, h),0,0,INTER_LINEAR);
}

// top = (int) (0.05*src.rows); bottom = top;
top = 50; bottom =50;
copyMakeBorder( src, dst, top, bottom, 0, 0, BORDER_CONSTANT, Scalar( 0, 0, 0 ) );

  /// Convert it to gray
  cvtColor( dst, src_gray, COLOR_BGR2GRAY );
  /// equalizeHist( src_gray, src_gray );  // skc: not good
  BrightnessAndContrastAuto( src_gray, src_gray, 0 );

  /// Reduce the noise so we avoid false circle detection
  /// for a 640x480 image, use 9x9 blur
int blur_sz = src.cols * 9 / 640 ;
if(blur_sz>31)blur_sz=31;
cout << blur_sz << endl ;

  GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

  std::vector<Vec3f> circles;

  /// Apply the Hough Transform to find the circles
  HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 0, 0 );

  /// Draw the circles detected
if ( circles.size() == 1 )
  // for( size_t i = 0; i < circles.size(); i++ )
  {
int i = 0;
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( dst, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
cout << "centered at (" << circles[i][0] * ratio << "," << ( circles[i][1] - 50 ) * ratio << ")" << endl;
      circle( dst, center, radius, Scalar(0,0,255), 3, 8, 0 );
   }


  namedWindow( "Hough Circle Transform Demo", WINDOW_AUTOSIZE );
  imshow( "Hough Circle Transform Demo", dst);
  // imshow( "Auto contract result", src_gray);

  waitKey(0);
  return 0;
}

void BrightnessAndContrastAuto(const cv::Mat &src, cv::Mat &dst, float clipHistPercent=0)
{

    CV_Assert(clipHistPercent >= 0);
    CV_Assert((src.type() == CV_8UC1) || (src.type() == CV_8UC3) || (src.type() == CV_8UC4));

    int histSize = 256;
    float alpha, beta;
    double minGray = 0, maxGray = 0;

    //to calculate grayscale histogram
    cv::Mat gray;
    if (src.type() == CV_8UC1) gray = src;
    else if (src.type() == CV_8UC3) cvtColor(src, gray, COLOR_BGR2GRAY);
    else if (src.type() == CV_8UC4) cvtColor(src, gray, COLOR_BGRA2GRAY);
    if (clipHistPercent == 0)
    {
        // keep full available range
        cv::minMaxLoc(gray, &minGray, &maxGray);
    }
    else
    {
        cv::Mat hist; //the grayscale histogram

        float range[] = { 0, 256 };
        const float* histRange = { range };
        bool uniform = true;
        bool accumulate = false;
        calcHist(&gray, 1, 0, cv::Mat (), hist, 1, &histSize, &histRange, uniform, accumulate);

        // calculate cumulative distribution from the histogram
        std::vector<float> accumulator(histSize);
        accumulator[0] = hist.at<float>(0);
        for (int i = 1; i < histSize; i++)
        {
            accumulator[i] = accumulator[i - 1] + hist.at<float>(i);
        }

        // locate points that cuts at required value
        float max = accumulator.back();
        clipHistPercent *= (max / 100.0); //make percent as absolute
        clipHistPercent /= 2.0; // left and right wings
        // locate left cut
        minGray = 0;
        while (accumulator[minGray] < clipHistPercent)
            minGray++;

        // locate right cut
        maxGray = histSize - 1;
        while (accumulator[maxGray] >= (max - clipHistPercent))
            maxGray--;
    }

    // current range
    float inputRange = maxGray - minGray;

    alpha = (histSize - 1) / inputRange;   // alpha expands current range to histsize range
    beta = -minGray * alpha;             // beta shifts current range so that minGray will go to 0

    // Apply brightness and contrast normalization
    // convertTo operates with saurate_cast
    src.convertTo(dst, -1, alpha, beta);

    // restore alpha channel from source 
    if (dst.type() == CV_8UC4)
    {
        int from_to[] = { 3, 3};
        cv::mixChannels(&src, 4, &dst,1, from_to, 1);
    }
    return;
}
