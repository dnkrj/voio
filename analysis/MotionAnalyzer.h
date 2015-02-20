#ifndef _MOTION_ANALYZER
#define _MOTION_ANALYZER

#include "gifs/gifcv.h"

#ifndef _OPEN_CV
#define _OPEN_CV
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#endif

#include <string>
#include <vector>
#include "Strategy.h"

class MotionAnalyzer : Strategy {
	cv::VideoCapture cap;
	
	/*
	Produces set of timestamps to perform rolling window analysis on.
	May change overall format to roll through whole video, currently has an upper bound on time taken.
	*/
	void getWindows(double length, std::vector<Timestamp>& ts);
	
	/*
	Takes an input of a list of windowed segments (as timestamps), the video length and the required length,
	returning a list of timestamps (15 as a maximum). Performs optical flow calculations using OpenCV
	calcOpticalFlowPyrLK() and performs analysis on linear movement and rotational movement, currently
	arbitrarily weighted. Points to follow are constant for rotational movement and calculated using
	goodFeaturesToTrack() for linear movement.
	*/
	std::vector<Timestamp> finalFilter(std::vector<Timestamp>& ts, double length, double clipLength);
	
	/*
	Takes an input of a grid of vector field values between two frames and spacings in the x and y directions.
	Calculates approximations to the rotations of each point between frames using vector curl and returns the mean of these values.
	Edge points are not used as central difference numerical derivatives are calculated.
	*/
	double calcRotation(std::vector<cv::Point2f>& values, double dx, double dy);
	
	/*
	Numerical partial derivative of the y component of the field with respect to x.
	*/
	double gx(std::vector<cv::Point2f>& values, double delta, int x, int y);
	
	/*
	Numerical partial derivative of the x component of the field with respect to y.
	*/
	double fy(std::vector<cv::Point2f>& values, double delta, int x, int y);
	
	/*
	Fill features-to-track array with random with values, when no good features are found.
	*/
	void fillRandom(std::vector<cv::Point2f>& list, int amount, double width, double height);
	
	public:
		MotionAnalyzer();
		~MotionAnalyzer();
		
		/*
		Overrides abstract function to produce timestamps given an input video file.
		*/
		std::vector<Timestamp> processVideo(const std::string& filename, int secondsPerClip);
};
#endif
