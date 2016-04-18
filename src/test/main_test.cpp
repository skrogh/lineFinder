#include "LineFinder.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <Eigen/Core>

#define MAX_LINES (20)

int main(int argc, char const *argv[])
{
	
	cv::VideoCapture cap(0);
	if(!cap.isOpened())  // check if we succeeded
		return -1;
	cap.set(CV_CAP_PROP_FRAME_WIDTH,320);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,240);
	cap.set(CV_CAP_PROP_FPS,15);


	unsigned int count = 0;
	while (1) {
		count++;
		cv::Mat image;
		cap >> image;
		int width = 320;
		int height = 240;

		int lineX[MAX_LINES];
		int lineY[MAX_LINES];
		int lines;
		findLines( image, width, height, 150, lineX, lineY, &lines);

		
		for ( int j = 0; j < lines; j++ )
		{
			cv::circle( image, cv::Point(lineX[j], lineY[j]), 10, cv::Scalar(0,0,255));
		}

		int startX, startY, endX, endY;
		if ( findRightLine( lineX, lineY, lines, width, height, &startX, &startY, &endX, &endY ) )
		{
			double dist, theta;
			Eigen::Vector2d p_center_l, p_line_w;
			findDistTheta( startX, startY, endX, endY, width, height, &dist, &theta, p_center_l, p_line_w );
			cv::line( image, cv::Point(startX, startY), cv::Point(endX, endY), cv::Scalar(0,0,255));
			cv::line( image, cv::Point(width/2.0, height/2.0), cv::Point(width/2.0+p_line_w(0), height/2.0-p_line_w(1)), cv::Scalar(0,0,255));
			printf( "dist: %f, theta %f\n", dist, theta );
		}

		cv::imshow( "Display", image );
		cv::waitKey(1);                                          // Wait for a keystroke in the window
   

	};



	return 0;
}
