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


FaceDetector::FaceDetector(string file1, string file2) : face_cascade(file1), eye_cascade(file2){
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
void FaceDetector::detectAndMark(Mat &im){
    Mat grey;
    cvtColor(im, grey, COLOR_RGB2GRAY);
    Mat edges;
    GaussianBlur(grey, edges, Size(11,11), 13.0);
    Canny(edges, edges, 8.9, 6.0);
    vector<Rect> faces;
    face_cascade.detectMultiScale(grey, faces);
    auto first = faces.begin();
    auto last = faces.end();
    cvtColor(edges, edges, COLOR_GRAY2RGB);
    im+=edges;
    while (first!=last) {
        Mat reducedFace;
        reducedFace = grey.rowRange(first->y, first->y + first->height).colRange(first->x, first->x+first->width);
        Mat reducedFace2(reducedFace);
        vector<Rect> eyes;
        vector<Rect> smiles;
        eye_cascade.detectMultiScale(reducedFace, eyes);
        Scalar clr = CLR_blue;
        if (eyes.size() >= 2){
            clr = CLR_green;
        }
        rectangle(im, *first, clr);
        populateWithRectangles(eyes, im, CLR_red, first->tl());
        first++;
    }
    
};
