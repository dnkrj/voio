//
//  main.cpp
//  FunctionalityTesting
//
//  Created by Laurynas Karazija on 18/02/15.
//  Copyright (c) 2015 Laurynas Karazija. All rights reserved.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <CLMTracker.h>
using namespace std;
using namespace cv;


vector<string> get_arguments(int argc, const char **argv)
{
    
    vector<string> arguments;
    
    for(int i = 1; i < argc; ++i)
    {
        arguments.push_back(string(argv[i]));
    }
    return arguments;
}


int main(int argc, const char **argv) {
    int device = 0;
    string filename = "/Users/laurynaskarazija/videos/0294_02_004_angelina_jolie.avi";
    bool use_camera_plane_pose= false;
    vector<string> arguments = get_arguments(argc, argv);
    float fx = 500, fy = 500, cx = 0, cy = 0;
    CLMTracker::CLMParameters clm_parameters(arguments);
    CLMTracker::get_camera_params(device, fx, fy, cx, cy, arguments);
    CLMTracker::CLM clm_model(clm_parameters.model_location);
    
    bool cx_undefined = false;
    if(cx == 0 || cy == 0)
    {
        cx_undefined = true;
    }
    
    Mat captured_image;
    namedWindow("CLM Tracking", 1);
    
    
    if(cx_undefined)
    {
        cx = captured_image.cols / 2.0f;
        cy = captured_image.rows / 2.0f;
    }
    
    
    int64 t1,t0 = cv::getTickCount();
    double fps = 10;
    
    int frame_count = 0;
    
    VideoCapture video_capture(filename); // open the default camera with devive otherwise testfile
    if(!video_capture.isOpened())  // check if we succeeded
        return -1;
    for(;;) {
        int tries = 20;
            do {
                video_capture >> captured_image;}
        while(captured_image.empty() && (tries-- >0));
        // Reading the images
        Mat_<float> depth_image;
        Mat_<uchar> grayscale_image;
        
        if(captured_image.channels() == 3)
        {
            cvtColor(captured_image, grayscale_image, CV_BGR2GRAY);
        }
        else
        {
            grayscale_image = captured_image.clone();
        }
        
        // The actual facial landmark detection / tracking
        bool detection_success = CLMTracker::DetectLandmarksInVideo(grayscale_image, depth_image, clm_model, clm_parameters);
        
        // Work out the pose of the head from the tracked model
        Vec6d pose_estimate_CLM;
        if(use_camera_plane_pose)
        {
            pose_estimate_CLM = CLMTracker::GetCorrectedPoseCameraPlane(clm_model, fx, fy, cx, cy, clm_parameters);
        }
        else
        {
           pose_estimate_CLM = CLMTracker::GetCorrectedPoseCamera(clm_model, fx, fy, cx, cy, clm_parameters);
        }
        // Visualising the results
        // Drawing the facial landmarks on the face and the bounding box around it if tracking is successful and initialised
        double detection_certainty = clm_model.detection_certainty;
        
        double visualisation_boundary = 0.2;
        
        // Only draw if the reliability is reasonable, the value is slightly ad-hoc
        if(detection_certainty < visualisation_boundary)
        {
            CLMTracker::Draw(captured_image, clm_model);
            
            if(detection_certainty > 1)
                detection_certainty = 1;
            if(detection_certainty < -1)
                detection_certainty = -1;
            
            detection_certainty = (detection_certainty + 1)/(visualisation_boundary +1);
            
            // A rough heuristic for box around the face width
            int thickness = (int)std::ceil(2.0* ((double)captured_image.cols) / 640.0);
            
            Vec6d pose_estimate_to_draw = CLMTracker::GetCorrectedPoseCameraPlane(clm_model, fx, fy, cx, cy, clm_parameters);
            
            // Draw it in reddish if uncertain, blueish if certain
            CLMTracker::DrawBox(captured_image, pose_estimate_to_draw, Scalar((1-detection_certainty)*255.0,0, detection_certainty*255), thickness, fx, fy, cx, cy);
            
        }
        
        // Work out the framerate
        if(frame_count % 10 == 0)
        {
            t1 = cv::getTickCount();
            fps = 10.0 / (double(t1-t0)/cv::getTickFrequency());
            t0 = t1;
        }
        
        char fpsC[255];
        sprintf(fpsC, "%d", (int)fps);
        string fpsSt("FPS:");
        fpsSt += fpsC;
        cv::putText(captured_image, fpsSt, cv::Point(10,20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255,0,0));
        
        imshow("CLM Tracking", captured_image);
        if(waitKey(30) >= 0) break;
        frame_count++;
    }
    return 0;
}
