#include "LineFinder.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry> 
#include <cmath> 

void
findLines( const cv::Mat& image, int width, int height, int edges, int lineX[MAX_LINES], int lineY[MAX_LINES], int* lines_ )
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
	*lines_ = lines;
}

bool
findRightLine( int lineX[], int lineY[], int lines, int width, int height,
	int* startX, int* startY, int* endX, int* endY )
{
	// Extract right side of line (get line in top and bottom)
	int maxTop = -1;
	int maxTopJ = -1;
	int maxBot = -1;
	int maxBotJ = -1;
	for ( int j = 0; j < lines; j++ )
	{
		if ( lineY[j] > height/2 )
		{
			if( lineX[j]>maxTop )
			{
				maxTop = lineX[j];
				maxTopJ = j;
			}
		}
		else
		{
			if( lineX[j]>maxBot )
			{
				maxBot = lineX[j];
				maxBotJ = j;
			}
		}

	}
	if ( maxTopJ>=0 && maxBotJ>=0 )
	{
		*startX = lineX[maxBotJ];
		*startY = lineY[maxBotJ];
		*endX = lineX[maxTopJ];
		*endY = lineY[maxTopJ];
		return true;
	}
	return false;
}

void
findDistTheta( double startX, double startY, double endX, double endY, double width, double height,
	double* dist, double* theta, Eigen::Vector2d& p_center_l, Eigen::Vector2d& p_line_w )
{
	startY = height - 1 - startY;
	endY = height - 1 - endY;

	Eigen::Vector2d start(startX,startY);
	Eigen::Vector2d end(endX,endY);
	Eigen::Vector2d center(width/2.0,height/2.0);

	Eigen::Vector2d p_center_w = center - start;
	Eigen::Vector2d l_direction = end - start;
	l_direction.normalize();

	Eigen::Matrix2d R_w_l;
	R_w_l
		<< l_direction(0) , -l_direction(1)
		,  l_direction(1) ,  l_direction(0);

	p_center_l = R_w_l.transpose() * p_center_w;
	p_line_w << 0.0 , -p_center_l(1);
	p_line_w = R_w_l*p_line_w;
	*dist = p_center_l(1);
	*theta = atan2(l_direction(0),-l_direction(1));
}