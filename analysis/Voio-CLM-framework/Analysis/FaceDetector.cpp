//
//  FaceDetector.cpp
//  OpenCVTest2
//
//  Created by Laurynas Karazija on 05/02/15.
//  Copyright (c) 2015 Laurynas Karazija. All rights reserved.
//

#include "FaceDetector.h"
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


FaceDetector::FaceDetector(string face, string eyes, string nose, string mouth)
: face_cascade(face), eye_cascade(eyes), nose_cascade(nose), mouth_cascade(mouth)
{
};
FaceDetector::~FaceDetector(){
};

void FaceDetector::populateWithRectangles(const vector<Rect> &vkt, Mat &img, const Scalar &clr, const Point &tl ){
    auto first = vkt.begin();
    auto last = vkt.end();
    while (first!=last){
        Rect add = *first;
        add.x+=tl.x;
        add.y+=tl.y;
        rectangle(img, add, clr);
        first++;
    }
};
void FaceDetector::detectAndMark(Mat &im){
    Mat grey;
    cvtColor(im, grey, COLOR_RGB2GRAY);
    //Mat edges;
    vector<Rect> faces;
    face_cascade.detectMultiScale(grey, faces);
    auto first = faces.begin();
    auto last = faces.end();
    while (first!=last) {
        Mat reducedFace;
        reducedFace = grey.rowRange(first->y, first->y + first->height).colRange(first->x, first->x + first->width);
        vector<Rect> eyeV;
        vector<Rect> noseV;
        vector<Rect> mouthV;
        eye_cascade.detectMultiScale(reducedFace, eyeV);
        nose_cascade.detectMultiScale(reducedFace, noseV);
        mouth_cascade.detectMultiScale(reducedFace,mouthV);
        if (eyeV.size() >= 2, noseV.size() >0, mouthV.size()>0){
            //proper face
            rectangle(im, *first, CLR_green);
            populateWithRectangles(eyeV, im, CLR_red, first->tl());
            populateWithRectangles(noseV, im, CLR_red, first->tl());
            populateWithRectangles(mouthV, im, CLR_red, first->tl());
        }
        rectangle(im, *first, CLR_blue);
        first++;
    }
    
};
