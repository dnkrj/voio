#include "MotionAnalyzer.h"

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

using namespace cv;

MotionAnalyzer::MotionAnalyzer() {}
double MotionAnalyzer::movementCoefficient(double start, double end, double minThresh, double maxThresh) {

    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;

    if(!cap.isOpened()) throw "Cannot open file.";
    
    Mat gray, prevGray, image, diffx, diffy, mag;
    std::vector<Point2f> points[2];
    std::vector<double> movements;
    cap.set(CV_CAP_PROP_POS_MSEC, start);
    Mat f;
    cap >> f;
    if(f.empty()) throw "Empty frames.";
	f.copyTo(image);
	cvtColor(image, prevGray, COLOR_BGR2GRAY);
	
	goodFeaturesToTrack(prevGray, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
	cornerSubPix(prevGray, points[0], subPixWinSize, Size(-1,-1), termcrit);
    
    while(cap.get(CV_CAP_PROP_POS_MSEC)<end) {
		Mat frame;
		cap >> frame;
    	if(frame.empty()) throw "Empty frames.";
    	
		frame.copyTo(image);
    	cvtColor(image, gray, COLOR_BGR2GRAY);
		std::vector<uchar> status;
		std::vector<float> err;
		if(prevGray.empty()) gray.copyTo(prevGray);
		calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);
		Mat xptsA(points[0].size(), 1, CV_32F, &points[0][0].x, 2 * sizeof(float));
		Mat xptsB(points[0].size(), 1, CV_32F, &points[1][0].x, 2 * sizeof(float));
		Mat yptsA(points[0].size(), 1, CV_32F, &points[0][0].y, 2 * sizeof(float));
		Mat yptsB(points[0].size(), 1, CV_32F, &points[1][0].y, 2 * sizeof(float));
		subtract(xptsA, xptsB, diffx);
		subtract(yptsA, yptsB, diffy);	
    	magnitude(diffx, diffy, mag);
    	double val = mean(mag)[0];
    	if(val<maxThresh && val>minThresh) {
    		movements.push_back(val);
    	}
    	
    	gray.copyTo(prevGray);
    	std::swap(points[0], points[1]);
    }
    
	return mean(movements)[0];
}

double MotionAnalyzer::normIndex(double start, double end, double minThresh, double maxThresh) {

    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    double count = 0;

    if(!cap.isOpened()) throw "Cannot open file.";
    
    Mat gray, prevGray, image, diffx, diffy, mag;
    std::vector<Point2f> points[2];
    std::vector<double> movements;
    cap.set(CV_CAP_PROP_POS_MSEC, start);
    Mat f;
    cap >> f;
    if(f.empty()) throw "Empty frames.";
	f.copyTo(image);
	cvtColor(image, prevGray, COLOR_BGR2GRAY);
	
	goodFeaturesToTrack(prevGray, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
	cornerSubPix(prevGray, points[0], subPixWinSize, Size(-1,-1), termcrit);
    
    while(cap.get(CV_CAP_PROP_POS_MSEC)<end) {
		Mat frame;
		cap >> frame;
    	if(frame.empty()) throw "Empty frames.";
    	
		frame.copyTo(image);
    	cvtColor(image, gray, COLOR_BGR2GRAY);
		std::vector<uchar> status;
		std::vector<float> err;
		if(prevGray.empty()) gray.copyTo(prevGray);
		calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);
		Mat xptsA(points[0].size(), 1, CV_32F, &points[0][0].x, 2 * sizeof(float));
		Mat xptsB(points[0].size(), 1, CV_32F, &points[1][0].x, 2 * sizeof(float));
		Mat yptsA(points[0].size(), 1, CV_32F, &points[0][0].y, 2 * sizeof(float));
		Mat yptsB(points[0].size(), 1, CV_32F, &points[1][0].y, 2 * sizeof(float));
		subtract(xptsA, xptsB, diffx);
		subtract(yptsA, yptsB, diffy);	
    	magnitude(diffx, diffy, mag);
    	
    	double val = mean(mag)[0];
    	if(val<maxThresh && val>minThresh) count += val;
    	
    	gray.copyTo(prevGray);
    	std::swap(points[0], points[1]);
    }
    
    if(count == 0) return 1E50;
	else return 1/count;
}
std::vector<Timestamp> MotionAnalyzer::processVideo(const std::string& filename) {
	cap.open(filename);
	if(!cap.open(filename)) throw "Error opening file.";
	else {
		cap.set(CV_CAP_PROP_POS_AVI_RATIO, 1);
		double length = cap.get(CV_CAP_PROP_POS_MSEC);
		double num;
		if(length>60000) num = length/50;
		else num = 1500;
		cap.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
		std::vector<Timestamp> ret;
		std::vector<double> values;
		std::map<double, Timestamp> func;
		
		for(double x = 0; x<length-3000; x += num) {
			values.push_back(movementCoefficient(x, x + 3000, 1, 100));
		}
		
		double mn = mean(values)[0];
		double min = mn/3;
		double max = mn*3;
		
		for(double x = 0; x<length-3000; x += num) {
			func.insert(std::pair<double, Timestamp>(normIndex(x, x + 3000, min, max), Timestamp(x, x + 3000)));
		}
		
		int i = 0;
		for(auto& kv : func) {
			if(i>6) break;
			ret.push_back(kv.second);
			i++;
		}
		return ret;
	}
}
