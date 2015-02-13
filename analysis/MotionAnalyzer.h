#ifndef _MOTION_ANALYZER
#define _MOTION_ANALYZER

#include "gifs/gifcv.h"

#ifndef _OPEN_CV
#define _OPEN_CV
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#endif

#include <string>
#include <vector>

class MotionAnalyzer {
	cv::VideoCapture cap;
	double movementCoefficient(double start, double end, double minThresh, double maxThresh);
	double normIndex(double start, double end, double minThresh, double maxThresh);
	public:
		MotionAnalyzer();
		std::vector<Timestamp> processVideo(const std::string& filename);
};
#endif
