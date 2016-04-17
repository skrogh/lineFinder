#include "LineFinder.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"

void
findLines( const cv::Mat& image, int width, int height, int edges, int lineX[MAX_LINES], int lineY[MAX_LINES] )
{
	int borderLength = width*2 + height*2 - 4;


	cv::Mat border(1, borderLength + edges*2, CV_8UC3);

	int i = edges;
	for ( int j = 0; j < width; j++ )
		border.at<cv::Vec3b>(0,i++) = image.at<cv::Vec3b>(0,j);
	for ( int j = 1; j < height; j++ )
		border.at<cv::Vec3b>(0,i++) = image.at<cv::Vec3b>(j,width-1);
	for ( int j = width-2; j >= 0; j-- )
		border.at<cv::Vec3b>(0,i++) = image.at<cv::Vec3b>(height-1,j);
	for ( int j = height-2; j > 0; j-- )
		border.at<cv::Vec3b>(0,i++) = image.at<cv::Vec3b>(j,0);
	i = 0;
	int h = borderLength + edges*2 - 1;
	int k = borderLength + edges - 1;
	for ( int j = 0; j > edges; j++ )
	{
		border.at<cv::Vec3b>(0,i++) = border.at<cv::Vec3b>(0,j);
		border.at<cv::Vec3b>(0,h--) = border.at<cv::Vec3b>(0,k--);
		i++;
	}

	cv::Mat borderBW;
	cvtColor(border, borderBW, CV_RGB2GRAY);
	cv::subtract(255,borderBW,borderBW);
	cv::blur( borderBW, borderBW, cv::Size(4,1) );
	cv::blur( borderBW, borderBW, cv::Size(4,1) );
	cv::blur( borderBW, borderBW, cv::Size(4,1) );
	cv::Mat kernel100(1,100,CV_8UC1,cv::Scalar(255));
	cv::Mat kernel20(1,20,CV_8UC1,cv::Scalar(255));
	cv::Mat mask;
	cv::erode( borderBW, mask, kernel100, cv::Point(-1,-1), 1 );
	cv::blur( mask, mask, cv::Size(50,1) );
	cv::blur( mask, mask, cv::Size(50,1) );
	cv::Mat line;
	cv::subtract(borderBW,mask,mask);
	cv::erode( mask, mask, kernel20, cv::Point(-1,-1), 1 );
	double min;
	double max;
	minMaxLoc( mask, &min, &max );
	cv::threshold(mask, line, 255-(255-min)*0.7, 255, cv::THRESH_BINARY_INV);


	bool lineStarted = false;
	int lineStart = 0;
	int lineEnd = 0;
	int lineCenter[MAX_LINES];
	int lines = 0;
	for ( int j = 0; j < borderLength + edges*2; j++ )
	{
		int value = line.at<unsigned char>(0,j);
		if ( value == 0 )
		{
			if ( !lineStarted )
				lineStart = j;
			lineStarted = true;
		}
		else
		{
			if ( lineStarted )
			{
				lineEnd = j;
				
				int linePos = (lineStart + lineEnd)/2.0;
				if ( ( linePos >= edges) && ( linePos <= borderLength + edges) )
				{
					linePos -= edges;
					lineCenter[lines] = linePos;
					lineX[lines] =(linePos<width-1)?
						linePos
						:(linePos<width+height-2)?
						width-1
						:(linePos<width*2+height-3)?
						width*2+height-linePos-3
						:
						0;
					lineY[lines] = (linePos<width-1)?
						0
						:(linePos<width+height-2)?
						linePos-width+1
						:(linePos<width*2+height-3)?
						height-1
						:
						width*2+height*2-linePos-4;
					lines++;
				}
			}

			lineStarted = false;
		}
		if ( lines > (MAX_LINES-2) )
			break;
	}
}
