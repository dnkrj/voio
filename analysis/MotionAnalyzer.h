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
#include "Strategy.h"

class MotionAnalyzer : Strategy {
	cv::VideoCapture cap;
	//double movementCoefficient(double start, double end, double minThresh, double maxThresh);
	//std::pair<double, Timestamp> normIndex(double start, double end, double mn);
	//void process(std::map<double, Timestamp>& func, double mn, double windowSize, double length);
	Timestamp expand(Timestamp t, double length);
	void getWindows(double length, std::vector<Timestamp>& ts);
	double preProcess(std::vector<Timestamp>& ts, double minThresh, double maxThresh);
	std::vector<Timestamp> expandWindows(std::vector<Timestamp>& ts, double mn, double length);
	std::vector<Timestamp> findBest(std::vector<Timestamp>& ts, double mn, double length);
	std::vector<Timestamp> finalFilter(std::vector<Timestamp>& ts, double length, double clipLength);
	public:
		MotionAnalyzer();
		~MotionAnalyzer();
		std::vector<Timestamp> processVideo(const std::string& filename, int secondsPerClip);
};
#endif
