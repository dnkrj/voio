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
#include <cmath>

using namespace cv;

//const double ws;
const int nX = 25;
const int nY = 25;

MotionAnalyzer::MotionAnalyzer() {}

MotionAnalyzer::~MotionAnalyzer() {}

Timestamp MotionAnalyzer::expand(Timestamp t, double length) {
	double x, y;
	if(t.getStart()>2500) x = t.getStart() - 2500;
	else x = t.getStart();
	if(t.getEnd()<length - 2500) y = t.getEnd() + 2500;
	else y = t.getEnd();
	return Timestamp(x, y);
}

void MotionAnalyzer::getWindows(double length, std::vector<Timestamp>& ts) {
	ts.clear();
	if(length<60000) {
		for(double x = 0; x<length - 5000; x += 3500) {
			ts.push_back(Timestamp(x, x + 5000));
		}
	}
	else if(length<600000) {
		for(double x = 0; x<length - 5000; x += 4500) {
			ts.push_back(Timestamp(x, x + 5000));
		}
	} else {
		double num = length/300;
		for(double x = 0; x<length - 5000; x += num) {
			ts.push_back(Timestamp(x, x + 5000));
		}
	}
}

double MotionAnalyzer::preProcess(std::vector<Timestamp>& ts, double minThresh, double maxThresh) {
	
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS, 20 ,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    double sum = 0;
    double N = 0;
    double start;
    double end;

    if(!cap.isOpened()) throw "Cannot open file.";
    for(unsigned int i = 0; i<ts.size(); i++, N++) {
    	start = ts[i].getStart();
    	end = ts[i].getEnd();
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
		sum += mean(movements)[0];
    }
	return sum/N;
}

static double update(std::vector<double>& values, double& sum, double value, unsigned int& index) {	
	if(index>=values.size()) index = 0;
	sum -= values[index];
	values[index] = value;
	sum += value;
	return sum;
}

std::vector<Timestamp> MotionAnalyzer::expandWindows(std::vector<Timestamp>& ts, double mn, double length) {
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS, 20, 0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    double count = 0;
    std::map<double, Timestamp> func;
    std::vector<Timestamp> ret;

    if(!cap.isOpened()) throw "Cannot open file.";
    for(unsigned int i = 0; i<ts.size(); i++) {
    	double start = ts[i].getStart();
    	double end = ts[i].getEnd();
		Mat gray, prevGray, image, diffx, diffy, mag;
		std::vector<Point2f> points[2];
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
			if(val>mn) count += val;
			
			gray.copyTo(prevGray);
			std::swap(points[0], points[1]);
		}
		if(count < 0.05) {
			func.insert(std::pair<double, Timestamp>(1E100, Timestamp(start, end)));
		} else func.insert(std::pair<double, Timestamp>(1/count, Timestamp(start, end)));
	}
	int i = 0;
	for(auto& kv : func) {
		if(i>14) break;
		//std::cout << kv.first << std::endl;
		ret.push_back(expand(kv.second, length));
		i++;
	}
	return ret;
}

//Precondition: points are ordered as {{(x1,y1), (x2,y1), ...}, {(x1,y2), (x2,y2), ...}, ...}
double MotionAnalyzer::calcRotation(std::vector<Point2f>& values, double dx, double dy) {
	std::vector<double> vals;
	for(int y = 2; y<nY-2; y++) {
		for(int x = 2; x<nX-2; x++) {
			vals.push_back(std::abs(gx(values, dx, x, y) - fy(values, dy, x, y)));
		}
	}
	return mean(vals)[0];
}

double MotionAnalyzer::gx(std::vector<Point2f>& values, double delta, int x, int y) {
	return (values[y*nX + x + 1].y - values[y*nX + x - 1].y)/(2*delta);
}

double MotionAnalyzer::fy(std::vector<Point2f>& values, double delta, int x, int y) {
	return (values[(y + 1)*nX + x].x - values[(y - 1)*nX + x].x)/(2*delta);
}

std::vector<Timestamp> MotionAnalyzer::finalFilter(std::vector<Timestamp>& ts, double length, double clipLength) {
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS, 20, 0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    std::map<double, Timestamp> func;
    std::vector<Timestamp> ret;
    std::vector<double> values;
    std::vector<uchar> status;
    std::vector<Point2f> fields[2];
	std::vector<float> err;
    Timestamp best;
    double sumr = 0;
    unsigned int index = 0;

    if(!cap.isOpened()) throw "Cannot open file.";
    
    Mat temp;
	cap >> temp;
	if(temp.empty()) throw "Empty frames.";

	Size sz = temp.size();
	double width = double(sz.width);
	double height = double(sz.height);
	double dx = width/double(nX);
	double dy = height/double(nY);
	for(int y = 0; y<nY; y++) {
		for(int x = 0; x<nX; x++) {
			fields[0].push_back(Point2f(float(x*dx), float(y*dy)));
		}
	}
    
    for(unsigned int i = 0; i<ts.size(); i++) {
    	double sum = 0;
    	double start = ts[i].getStart();
    	double end = ts[i].getEnd();
    	std::cout << "Analyzing: " << start << ", " << end << std::endl;
		Mat gray, prevGray, image, diffx, diffy, mag;
		std::vector<Point2f> points[2];
		cap.set(CV_CAP_PROP_POS_MSEC, start);
		
		Mat f;
		cap >> f;
		if(f.empty()) throw "Empty frames.";
	
		f.copyTo(image);
		cvtColor(image, prevGray, COLOR_BGR2GRAY);
	
		goodFeaturesToTrack(prevGray, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
		cornerSubPix(prevGray, points[0], subPixWinSize, Size(-1,-1), termcrit);

	
		while(cap.get(CV_CAP_PROP_POS_MSEC)<end && cap.get(CV_CAP_PROP_POS_MSEC)<clipLength + start) {
			cap >> f;
			if(f.empty()) throw "Empty frames.";
	  
			f.copyTo(image);
			cvtColor(image, gray, COLOR_BGR2GRAY);

			if(prevGray.empty()) gray.copyTo(prevGray);
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);
			calcOpticalFlowPyrLK(prevGray, gray, fields[0], fields[1], status, err, winSize, 3, termcrit, 0, 0.001);
			Mat xptsA(points[0].size(), 1, CV_32F, &points[0][0].x, 2 * sizeof(float));
			Mat xptsB(points[0].size(), 1, CV_32F, &points[1][0].x, 2 * sizeof(float));
			Mat yptsA(points[0].size(), 1, CV_32F, &points[0][0].y, 2 * sizeof(float));
			Mat yptsB(points[0].size(), 1, CV_32F, &points[1][0].y, 2 * sizeof(float));
			subtract(xptsA, xptsB, diffx);
			subtract(yptsA, yptsB, diffy);	
			magnitude(diffx, diffy, mag);
			
			double val = 0.5*mean(mag)[0] + 0.5*calcRotation(fields[1], dx, dy);
			sum += val;
			values.push_back(val);
			index++;
			
			gray.copyTo(prevGray);
			std::swap(points[0], points[1]);
		}
		
		best = Timestamp(start, cap.get(CV_CAP_PROP_POS_MSEC));
		sumr = sum;
		
		int round = 0;
		while(cap.get(CV_CAP_PROP_POS_MSEC)<end) {
			cap >> f;
			if(f.empty()) throw "Empty frames.";
			f.copyTo(image);
			cvtColor(image, prevGray, COLOR_BGR2GRAY);
	
			goodFeaturesToTrack(prevGray, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			cornerSubPix(prevGray, points[0], subPixWinSize, Size(-1,-1), termcrit);
						
			while(round<4) {
				cap >> f;
				if(f.empty()) throw "Empty frames.";
				f.copyTo(image);
				cvtColor(image, gray, COLOR_BGR2GRAY);

				if(prevGray.empty()) gray.copyTo(prevGray);
				calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);
				calcOpticalFlowPyrLK(prevGray, gray, fields[0], fields[1], status, err, winSize, 3, termcrit, 0, 0.001);
				Mat xptsA(points[0].size(), 1, CV_32F, &points[0][0].x, 2 * sizeof(float));
				Mat xptsB(points[0].size(), 1, CV_32F, &points[1][0].x, 2 * sizeof(float));
				Mat yptsA(points[0].size(), 1, CV_32F, &points[0][0].y, 2 * sizeof(float));
				Mat yptsB(points[0].size(), 1, CV_32F, &points[1][0].y, 2 * sizeof(float));
				subtract(xptsA, xptsB, diffx);
				subtract(yptsA, yptsB, diffy);	
				magnitude(diffx, diffy, mag);
				//std::cout << "Round: " << round << std::endl;
				
				double val = 0.5*mean(mag)[0] + 0.5*calcRotation(fields[1], dx, dy);
				update(values, sum, val, index);
				index++;
				if(sum>sumr) {
					double time = cap.get(CV_CAP_PROP_POS_MSEC);
					best = Timestamp(time - clipLength, time);
					sumr = sum;
				}
			
				gray.copyTo(prevGray);
				std::swap(points[0], points[1]);
				round++;
			}
			round = 0;
		}
		
		func.insert(std::pair<double, Timestamp>(1/sumr, best));
	}
	int i = 0;
	for(auto& kv : func) {
		if(i>14) break;
		std::cout << kv.first << std::endl;
		ret.push_back(kv.second);
		i++;
	}
	std::cout << "Saving GIFs." << std::endl;
	return ret;
}

std::vector<Timestamp> MotionAnalyzer::processVideo(const std::string& filename, int secondsPerClip) {
	cap.open(filename);
	if(!cap.open(filename)) throw "Error opening file.";
	else {
		cap.set(CV_CAP_PROP_POS_AVI_RATIO, 1);
		double length = cap.get(CV_CAP_PROP_POS_MSEC);
		double clipLen = double(secondsPerClip)*1000;
		cap.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
		std::cout << "Creating windows." << std::endl;
		std::vector<Timestamp> windows;
		getWindows(length, windows);
		//std::cout << "Performing preprocessing." << std::endl;
		//double mn = preProcess(windows, 1, 110);
		std::cout << "Performing final analysis." << std::endl;
		return finalFilter(windows, length, clipLen);
	}
}
