#include "LineFinder.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>

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

		int lineX[MAX_LINES];
		int lineY[MAX_LINES];
		int lines;
		findLines( image, 320, 240, 150, lineX, lineY, &lines);

		
		for ( int j = 0; j < lines; j++ )
		{
			circle( image, cv::Point(lineX[j], lineY[j]), 10, cv::Scalar(0,0,255));
		}
		cv::imshow( "Display", image );
		cv::waitKey(1);                                          // Wait for a keystroke in the window
   

	};



	return 0;
}
