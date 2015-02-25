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


FaceDetector::FaceDetector(string face, string eye, string mouth, string nose)
: face_cascade(face), eye_cascade(eye), mouth_cascade(mouth), nose_cascade(nose){
    //nothing else
};
FaceDetector::~FaceDetector(){
    //empty for now
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
int FaceDetector::detectAndMark(Mat &im){
    Mat grey;
    int numOfGoodFaces = 0;
    cvtColor(im, grey, COLOR_RGB2GRAY);
    vector<Rect> faces;
    face_cascade.detectMultiScale(grey, faces);
    auto first = faces.begin();
    auto last = faces.end();
    while (first!=last) {
        Mat reducedFace;
        reducedFace = grey.rowRange(first->y, first->y + first->height).colRange(first->x, first->x + first->width);
        vector<Rect> eyes;
        vector<Rect> mouths;
        vector<Rect> noses;
        eye_cascade.detectMultiScale(reducedFace, eyes);
        mouth_cascade.detectMultiScale(reducedFace, mouths);
        nose_cascade.detectMultiScale(reducedFace, noses);
    
        if (eyes.size() >= 2 && mouths.size() > 0 && noses.size() >0){
           //proper face;
            populateWithRectangles(eyes, im, CLR_white, first->tl());
            populateWithRectangles(mouths, im, CLR_yellow, first->tl());
            populateWithRectangles(noses, im, CLR_red, first->tl());
            rectangle(im, *first, CLR_green);
            numOfGoodFaces++;
        }
        else rectangle(im, *first, CLR_blue);
        first++;
    }
    return numOfGoodFaces;
};
