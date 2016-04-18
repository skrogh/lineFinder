#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <Eigen/Core>

#define MAX_LINES (20)

void findLines( const cv::Mat& image, int width, int height, int edges, int lineX[MAX_LINES], int lineY[MAX_LINES], int* lines_);

bool
findRightLine( int lineX[], int lineY[], int lines, int width, int height,
	int* startX, int* startY, int* endX, int* endY );
void
findDistTheta( double startX, double startY, double endX, double endY, double width, double height,
	double* dist, double* theta, Eigen::Vector2d& p_center_l, Eigen::Vector2d& p_line_w );