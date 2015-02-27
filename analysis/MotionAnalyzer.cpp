#include "MotionAnalyzer.h"

#include "gifs/timestamp.h"

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
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace cv;

//const double ws;
/*
Number of grid points is nX*nY.
*/
const int nX = 32;
const int nY = 32;

MotionAnalyzer::MotionAnalyzer() {
	std::srand(std::time(0));
}

MotionAnalyzer::~MotionAnalyzer() {}

void MotionAnalyzer::getWindows(double length, std::vector<Timestamp>& ts) {
	ts.clear();
	if(length<15000) {
		for(double x = 0; x<length - 4000; x += 1000) {
			ts.push_back(Timestamp(x, x + 4000));
		}
	} else if(length<60000) {
		for(double x = 0; x<length - 5000; x += 3000) {
			ts.push_back(Timestamp(x, x + 5000));
		}
	}
	else if(length<3000000) {
		for(double x = 0; x<length - 5000; x += 4000) {
			ts.push_back(Timestamp(x, x + 5000));
		}
	} else {
		double num = length/300;
		for(double x = 0; x<length - 5000; x += num) {
			ts.push_back(Timestamp(x, x + 5000));
		}
	}
}

static void update(std::vector<double>& values, double& sum, double value, unsigned int& index) {	
	if(index>=values.size()) index = 0;
	sum -= values[index];
	values[index] = value;
	sum += value;
}

//Precondition: points are ordered as {{(x1,y1), (x2,y1), ...}, {(x1,y2), (x2,y2), ...}, ...}
double MotionAnalyzer::calcRotation(std::vector<Point2f>& values, std::vector<Point2f>& oldvalues, double dx, double dy) {
	std::vector<double> vals;
	for(int y = 2; y<nY-2; y++) {
		for(int x = 2; x<nX-2; x++) {
			vals.push_back(std::abs(gx(values, oldvalues, dx, x, y) - fy(values, oldvalues, dy, x, y)));
		}
	}
	return mean(vals)[0];
}

double MotionAnalyzer::gx(std::vector<Point2f>& values, std::vector<Point2f>& oldvalues, double delta, int x, int y) {
	return ((values[y*nX + x + 1].y - oldvalues[y*nX + x + 1].y) - (values[y*nX + x - 1].y - oldvalues[y*nX + x - 1].y))/(2*delta);
}

double MotionAnalyzer::fy(std::vector<Point2f>& values, std::vector<Point2f>& oldvalues, double delta, int x, int y) {
	return ((values[(y + 1)*nX + x].x - oldvalues[(y + 1)*nX + x].x) - (values[(y - 1)*nX + x].x - oldvalues[(y - 1)*nX + x].x))/(2*delta);
}

void MotionAnalyzer::fillRandom(std::vector<Point2f>& list, int amount, double width, double height) {
	//int minX = 1;
	//int minY = 1;
	int maxX = int(width) - 1;
	int maxY = int(height) - 1;
	for(int i = 0; i<amount; i++) {
		int randX = 1 + (std::rand() % (int) (maxX));
		int randY = 1 + (std::rand() % (int) (maxY));
		list.push_back(Point2f(float(randX), float(randY)));
	}
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
		if(points[0].size() < 1) fillRandom(points[0], 30, width, height);
		cornerSubPix(prevGray, points[0], subPixWinSize, Size(-1,-1), termcrit);

	
		while(cap.get(CV_CAP_PROP_POS_MSEC)<end && cap.get(CV_CAP_PROP_POS_MSEC)<clipLength + start) {
			cap >> f;
			if(f.empty()) {std::cout << "Empty frames" << std::endl; break;}
	  
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
			
			double val = 0.5*mean(mag)[0] + 0.5*calcRotation(fields[1], fields[0], dx, dy);
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
			if(f.empty()) {std::cout << "Empty frames" << std::endl; break;}
			f.copyTo(image);
			cvtColor(image, prevGray, COLOR_BGR2GRAY);
	
			goodFeaturesToTrack(prevGray, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			if(points[0].size() < 1) fillRandom(points[0], 30, width, height);
			cornerSubPix(prevGray, points[0], subPixWinSize, Size(-1,-1), termcrit);
						
			while(round<4) {
				cap >> f;
				if(f.empty()) {std::cout << "Empty frames" << std::endl; break;}
				f.copyTo(image);
				cvtColor(image, gray, COLOR_BGR2GRAY);
				//imshow("Testing", gray);
				//waitKey(30);

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
				double L = 0.35*mean(mag)[0];
				if(L > 30) L = 30;
				double R = 42*calcRotation(fields[1], fields[0], dx, dy);
				if(R > 30) R = 30;
				//std::cout << "Linear: " << L << std::endl;
				//std::cout << "Rotational: " << R << std::endl;
				double val = L + R;
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
	double minT;
	double normi;
	for(auto& kv : func) {
		if(i == 0) minT = kv.first;
		if(i>10) break;
		normi = minT/kv.first;
		std::cout << normi << std::endl;
		if(normi>0.67) ret.push_back(kv.second);
		i++;
	}
	
	std::cout << "Saving clips." << std::endl;
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
		std::cout << "Performing analysis." << std::endl;
		return finalFilter(windows, length, clipLen);
	}
}
