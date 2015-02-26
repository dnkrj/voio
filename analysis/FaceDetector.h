//
//  FaceDetector.h
//  OpenCVTest2
//
//  Created by Laurynas Karazija on 05/02/15.
//  Copyright (c) 2015 Laurynas Karazija. All rights reserved.
//

#ifndef __OpenCVTest2__FaceDetector__
#define __OpenCVTest2__FaceDetector__

#include <opencv2/opencv.hpp>

#endif /* defined(__OpenCVTest2__FaceDetector__) */
using namespace std;
using namespace cv;

class FaceDetector{
private:
    Scalar CLR_red = Scalar(0,0,255,1);
    Scalar CLR_blue = Scalar(255,0,0,1);
    Scalar CLR_green = Scalar(0,255,0,1);
    CascadeClassifier face_cascade;
    CascadeClassifier eye_cascade;
    CascadeClassifier nose_cascade;
    CascadeClassifier mouth_cascade;
    void populateWithRectangles(const vector<Rect> &vkt, Mat &img, const Scalar &clr, const Point &tl );
public:
    FaceDetector(string face="haarcascade_frontalface_default.xml",
                 string eyes="haarcascade_eye.xml"
                 string nose="haarcascade_nose.xml"
                 string mouth="haarcascade_mouth.xml");
    ~FaceDetector();
    //actural work done here
    void detectAndMark(Mat &im);
};
