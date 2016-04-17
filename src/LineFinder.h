#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"

#define MAX_LINES (20)

void findLines( const cv::Mat& image, int width, int height, int edges, int lineX[MAX_LINES], int lineY[MAX_LINES], int* lines_);