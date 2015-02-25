//
//  analysis.cpp
//  VOIO_analysis
//
//  Created by Laurynas Karazija on 18/02/15.
//

#include <CLM.h>
#include <CLMTracker.h>
#include <CLMParameters.h>
#include <CLM_utils.h>

#include <fstream>
#include <sstream>
#include <array>
#include <cmath>

#include <cv.h>

#include<tbb/tbb.h>

#define GIF_L 2
#define NUM_Faces 8
using namespace std;
using namespace cv;

bool verbose = false;
bool output_to_file = false;




//weight function
double inline wF(array<double, 3> in ){
    // size times orientation
    return 0.8+(in[1] /0.4 * in[2]);
    // face taking 40% of the screen is massive :)
    // 0.8 - face pressence wiegth
    // certainty - unimportant - we care about displayable faces - HAAR gives wierd certainties
}

double inline get_orientation_weight(double alfa, double beta, double gama){
    //Strategy rotate 1 0 0 vector though said angles -> project to camera plane
    //Vec3d v(1.0, .0, .0);
    Mat_<double> v = Mat(3, 1, CV_64F);
    v.at<double>(0,0) = 1;
    v.at<double>(1,0) = 0;
    v.at<double>(2,0) = 0;
    Matx33d rot = CLMTracker::Euler2RotationMatrix(Vec3d(alfa, beta, gama));
    // Rotate the vector
    v = Mat(rot) * v;
    double x =v.at<double>(0,0);
    double y =v.at<double>(1,0);
    return sqrt(x*x + y*y);
}

vector<string> get_arguments(int argc, char **argv)
{
    
    vector<string> arguments;
    
    for(int i = 1; i < argc; ++i)
    {
        arguments.push_back(string(argv[i]));
    }
    return arguments;
}

void NonOverlapingDetections(const vector<CLMTracker::CLM>& clm_models, vector<Rect_<double> >& face_detections)
{
    
    // Go over the model and eliminate detections that are not informative (there already is a tracker there)
    for(size_t model = 0; model < clm_models.size(); ++model)
    {
        
        // See if the detections intersect
        Rect_<double> model_rect = clm_models[model].GetBoundingBox();
        
        for(int detection = face_detections.size()-1; detection >=0; --detection)
        {
            double intersection_area = (model_rect & face_detections[detection]).area();
            double union_area = model_rect.area() + face_detections[detection].area() - 2 * intersection_area;
            
            // If the model is already tracking what we're detecting ignore the detection, this is determined by amount of overlap
            if( intersection_area/union_area > 0.5)
            {
                face_detections.erase(face_detections.begin() + detection);
            }
        }
    }
}

bool runCLManalysis (vector<double> &result, double &framerate,string filename)
{
    vector<string> arguments;
    // Some initial parameters that can be overriden from command line
    // By default try webcam 0
    int device = 0;
    CLMTracker::CLMParameters clm_params(arguments);
    clm_params.use_face_template = true;
    // This is so that the model would not try re-initialising itself
    clm_params.reinit_video_every = -1;
    //very slow
    //clm_params.curr_face_detector = CLMTracker::CLMParameters::HOG_SVM_DETECTOR;
    //use instead
    clm_params.curr_face_detector = CLMTracker::CLMParameters::HAAR_DETECTOR;
    vector<CLMTracker::CLMParameters> clm_parameters;
    clm_parameters.push_back(clm_params);
    
    // The modules that are being used for tracking
    vector<CLMTracker::CLM> clm_models;
    vector<bool> active_models;
    
    
    //How MANY faces we track... SIGNIFICANT IMPACT ON PERFORMACE
    int num_faces_max = NUM_Faces;
    
    CLMTracker::CLM clm_model(clm_parameters[0].model_location);
    clm_model.face_detector_HAAR.load(clm_parameters[0].face_detector_location);
    clm_model.face_detector_location = clm_parameters[0].face_detector_location;
    
    clm_models.reserve(num_faces_max);
    
    clm_models.push_back(clm_model);
    active_models.push_back(false);
    
    for (int i = 1; i < num_faces_max; ++i)
    {
        clm_models.push_back(clm_model);
        active_models.push_back(false);
        clm_parameters.push_back(clm_params);
    }
    
    int frame_count = 0;
    
    // Do some grabbing
    VideoCapture video_capture(filename);
    if(!video_capture.isOpened())  // check if we succeeded
        return false;
    Mat captured_image;
    video_capture >> captured_image;
    frame_count++;
    //Some Constants:
    float cx = captured_image.cols / 2.0f;
    float cy = captured_image.rows / 2.0f;
    float fx = 600, fy = 600;
    int vidFC = video_capture.get(CV_CAP_PROP_FRAME_COUNT);
    framerate = video_capture.get(CV_CAP_PROP_FPS);
    
    ofstream out;
    out.open(filename + "_wav.txt");
    
    
    //Visualisations --------------
    int w =video_capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int h =video_capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    Size vidSize(w,h);
    //output file
    VideoWriter writerFace;
    writerFace = VideoWriter(filename+"_out.avi", video_capture.get(CV_CAP_PROP_FOURCC), 23.59, vidSize);
    if(!writerFace.isOpened())
    {
        cout<<"The output file is bad"<<endl;
    }
    namedWindow("tracking_result",1);
    Mat disp_image;
    //Mat waveform;
    //waveform = Mat::zeros( num_faces_max *3 *10,vidFC, CV_32S);
    //Visualisations --------------
    
    
    out<<num_faces_max<<endl;
    out<<vidFC<<endl;
    out<<framerate<<endl;
    result.clear();
    result.reserve(vidFC);
    Mat_<float> depth_image;
    Mat_<uchar> grayscale_image;
   // Starting tracking
    while(!captured_image.empty())
    {
        
        // Reading the images
       
        
        disp_image = captured_image.clone();
        
        if(captured_image.channels() == 3)
        {
            cvtColor(captured_image, grayscale_image, CV_BGR2GRAY);
        }
        else
        {
            grayscale_image = captured_image.clone();
        }
        
        
        vector<Rect_<double> > face_detections;
        
        bool all_models_active = true;
        for(unsigned int model = 0; model < clm_models.size(); ++model)
        {
            if(!active_models[model])
            {
                all_models_active = false;
            }
        }
        //SIGNIFICANT IMPACT ON PERFORMACE ---- when do we run facedetection?
        // Get the detections (every 8th frame and when there are free models available for tracking)
        if(frame_count % 8 == 0 && !all_models_active)
        {
            if(clm_parameters[0].curr_face_detector == CLMTracker::CLMParameters::HOG_SVM_DETECTOR)
            {
                vector<double> confidences;
                CLMTracker::DetectFacesHOG(face_detections, grayscale_image, clm_models[0].face_detector_HOG, confidences);
            }
            else
            {
                CLMTracker::DetectFaces(face_detections, grayscale_image, clm_models[0].face_detector_HAAR);
            }
            
        }
        
        // Keep only non overlapping detections (also convert to a concurrent vector
        NonOverlapingDetections(clm_models, face_detections);
        
        
        vector<tbb::atomic<bool> > face_detections_used(face_detections.size());
        
        // Go through every model and update the trackingpull out as a separate parallel/non-parallel method
        tbb::parallel_for(0, (int)clm_models.size(), [&](int model){
            bool detection_success = false;
            
            // If the current model has failed more than 4 times in a row, remove it
            if(clm_models[model].failures_in_a_row > 4)
            {
                active_models[model] = false;
                clm_models[model].Reset();
                
            }
            
            // If the model is inactive reactivate it with new detections
            if(!active_models[model])
            {
                
                for(size_t detection_ind = 0; detection_ind < face_detections.size(); ++detection_ind)
                {
                    // if it was not taken by another tracker take it (if it is false swap it to true and enter detection, this makes it parallel safe)
                    if(face_detections_used[detection_ind].compare_and_swap(true, false) == false)
                    {
                        
                        // Reinitialise the model
                        clm_models[model].Reset();
                        
                        // This ensures that a wider window is used for the initial landmark localisation
                        clm_models[model].detection_success = false;
                        detection_success = CLMTracker::DetectLandmarksInVideo(grayscale_image, depth_image, face_detections[detection_ind], clm_models[model], clm_parameters[model]);
                        
                        // This activates the model
                        active_models[model] = true;
                        
                        // break out of the loop as the tracker has been reinitialised
                        break;
                    }
                    
                }
            }
            else
            {
                // The actual facial landmark detection / tracking
                detection_success = CLMTracker::DetectLandmarksInVideo(grayscale_image, depth_image, clm_models[model], clm_parameters[model]);
            }
        });
        
        // STEP OF MAGIC ------------------------------------------ STEP OF MAGIC
        // Going through every model and visualising the results
        
        int num_active_models = 0;
        vector<std::array<double, 3> > frameFV;
        
        for(size_t model = 0; model < clm_models.size(); ++model)
        {
            // Visualising the results
            // Drawing the facial landmarks on the face and the bounding box around it if tracking is successful and initialised
            double detection_certainty = clm_models[model].detection_certainty;
            
            double visualisation_boundary = -0.1;
            
            // Only draw if the reliability is reasonable, the value is slightly ad-hoc
            if(detection_certainty < visualisation_boundary)
            {
                CLMTracker::Draw(disp_image, clm_models[model]);
                
                if(detection_certainty > 1)
                    detection_certainty = 1;
                if(detection_certainty < -1)
                    detection_certainty = -1;
                
                detection_certainty = (detection_certainty + 1)/(visualisation_boundary +1);
                
                // Work out the pose of the head from the tracked model
                Vec6d pose_estimate_CLM = CLMTracker::GetCorrectedPoseCameraPlane(clm_models[model], fx, fy, cx, cy, clm_parameters[model]);
                
                //double orientation = abs((1.0 - abs(pose_estimate_CLM[3]))*(1.0 - abs(pose_estimate_CLM[4])));
                double orientation = get_orientation_weight(pose_estimate_CLM[3], pose_estimate_CLM[4], pose_estimate_CLM[5]);
                
//
//                if(abs(pose_estimate_CLM[3]) <0.4 //Up and down looking
//                   && abs(pose_estimate_CLM[4] < 0.5)) //Left and right looking
//                   orientation =1;
                
                double face_size = clm_models[model].GetBoundingBox().area()/ (w * h);
                //cout<< model << " " <<orientation <<endl;
                array<double, 3> arr({{detection_certainty, face_size, orientation}});
                frameFV.push_back(arr);
                
                //Visualisations -------------- Begin
                int thickness = (int)std::ceil(2.0* ((double)captured_image.cols) / 640.0);
                // Draw it in reddish if uncertain, blueish if certain
                CLMTracker::DrawBox(disp_image, pose_estimate_CLM, Scalar(face_size*20 *255,orientation*255, detection_certainty *2 *255), thickness, fx, fy, cx, cy);
                //Visualisations -------------- End

                num_active_models++;
            }
        }
        double frameWeight = 0;
        for(int i =0; i<frameFV.size(); i++){
            frameWeight += wF(frameFV[i]);
        }
        out<<frameWeight<<endl;
        result.push_back(frameWeight);
    
        // STEP OF MAGIC ------------------END--------------------- STEP OF MAGIC
        
        
        //Visualisations -------------- BEGIN
        //waveform.at<int>(waveform.rows - (int(frameWeight*20) - 1), frame_count) = 255;
        //cout<<frameWeight*20<<endl;
        
        char progress[255];
        sprintf(progress, "%d // %d", frame_count, vidFC);
        string progress_s("Progress: ");
        progress_s += progress;
        putText(disp_image, progress_s, cv::Point(10,20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255,0,0));
        char active_models_char[255];
        sprintf(active_models_char, "%d", num_active_models);
        string active_models_string("Active models:");
        active_models_string += active_models_char;
        cv::putText(disp_image, active_models_string, cv::Point(10,40), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,0,255));
        for(int i = 0; i<frameFV.size(); i++){
            stringstream label;
            label<<"#"<<i<< " CERT: "<< int(frameFV[i][0]*100) <<"% SIZE: "<<frameFV[i][1]<<" ORTN: "<< frameFV[i][2];
            putText(disp_image, label.str(), cv::Point(10, h - 20 - i*20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(125,125,255));
        }
        stringstream wei;
        wei<<"Weight: "<<frameWeight;
        cv::putText(disp_image, wei.str(), cv::Point(10,60), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(125,0,255));
        imshow("tracking_result", disp_image);
        writerFace << disp_image;
        if(waitKey(30) >= 0){
            while (true) {
                if(waitKey(30) >= 0) break;
            }
        }
        //imwrite(filename+"_waveform.png", waveform);
        //Visualisations -------------- END
        video_capture >> captured_image;
        frame_count++;
    }
    out.close();
    //imwrite(filename+"_waveform.png", waveform);
    return true;
}
void smooth(const vector<double> &f, vector<double> &f_prime, int smooth_size=3){
    f_prime.clear();
    f_prime.reserve(f.size());
    for(int i = (smooth_size+1)/2 ; i < f.size() - (smooth_size+1)/2; i++){
        double value=0;
        for(int j = 0; j < smooth_size; j ++){
            value+= f[i - (smooth_size+1)/2 + j];
        }
        f_prime.push_back(value/smooth_size);
    }
}
struct Segment{
    int start;
    double sum;
};
bool getSegmentVector(const vector<double> &in, vector<Segment> &out, int segLen){
    //retuns sums of segments
    if (segLen > in.size()) return false;
    out.clear();
    out.reserve(in.size()-segLen);
    double sum =0;
    Segment s;
    for(int i = 0; i <segLen; i++){
        sum+=in[i];
    }
    s.start = 0;
    s.sum = sum;
    out.push_back(s);
    for(int i = segLen; i <in.size(); i++){
        Segment a;
        sum = sum + in[i] - in[i-segLen];
        a.start =i-segLen +1;
        a.sum = sum;
        out.push_back(a);
    }
    return true;
}
bool getMaxSeg(int &to_rtn, vector<Segment> &in, double sumTolerance,int segLen, int padding){
    //cout<<in.size()<<" ";
    auto first= in.begin();
    auto max = first;
    first++;
    auto end = in.end();
    if(in.size() < segLen)
        return false;
    while (first != end){
        if (first->sum >= sumTolerance)
            if(first->sum > max->sum){
                max = first;
            }
        first++;
    }
    
    to_rtn = max->start;
    if(to_rtn < sumTolerance){
        return false;
        //ran out of options;
    }
    auto erstart = max;
    while(erstart != in.begin()){
        if (erstart->start + padding < max->start) {
            erstart++;
            break;
        }
        erstart--;
    }
    auto erend = max+1;
    while(erend != in.end()){
        if (erend->start >= max->start+padding){
            break;
        }
        erend++;
    }
//    int i =1;
//    cout<<"start erase : ";
//    while(erstart!=erend){
//        cout<<i++<<" ";
//        in.erase(erstart++);
//    }
//    cout<<" size ";
    in.erase(erstart, erend);
    //cout<<in.size()<<endl;
    return true;
}
void FtoMat(const vector<double> &in, Mat &out){
    out = Mat::zeros( NUM_Faces *3 *10,in.size(), CV_32S);
    for(int frame_count = 0; frame_count < in.size(); frame_count++){
        out.at<double>(out.rows - (int)in[frame_count]*20 -1, frame_count) = 255;
    }
}
void derivative(const vector<double> &f, vector<double> &f_prime){
    f_prime.clear();
    f_prime.reserve(f.size()-1);
    for(int i =1; i < f.size(); i++){
        f_prime.push_back(f[i]-f[i-1]);
    }
}
int main (int argc, char **argv){
    string filename = "/Users/laurynaskarazija/Documents/TestVid.mp4";
    //string filename = "/Users/laurynaskarazija/videos/0217_03_006_alanis_morissette.avi";
    vector<double> clm_results;
    double fps =30;
    runCLManalysis(clm_results,fps, filename);
    int segLen = (int) fps * GIF_L;
//    int numFace, size;
//    ifstream in;
//    in.open(filename+"_wav.txt");
//    in>>numFace;
//    in>>size;
//    in>>fps;
//    //cout<<size<<endl;
//    for(int i = 0; i < size; i++){
//        double inp;
//        in>>inp;
//        //cout<<inp<<endl;
//        clm_results.push_back(inp);
//    }
//    in.close();
    cout<<"Analysis Complete"<<endl;
//    vector<double> clm_results_smoothed;
//    smooth(clm_results, clm_results_smoothed, 5);
//    int giflength_in_frames = (int) GIF_L*fps;
//    Mat displ;
//    FtoMat(clm_results_smoothed, displ);
//    namedWindow("smoothed", 1);
//    imshow("smoothed", displ);
//    imwrite(filename+"_waveform.png_smoothed.png", displ);
    vector<Segment> segmentVec;
    getSegmentVector(clm_results, segmentVec, segLen);
//    cout<<segmentVec.size()<<endl;
//    for(int i= 0; i < segmentVec.size(); i++){
//        cout<<i<<" "<< segmentVec[i].sum<<endl;
//    }
    ofstream output;
    output.open(filename+"_timestamps");
    int i=0;
    int start= 0;
    while(getMaxSeg(start, segmentVec, 0.95,segLen, segLen*3)){
        output<<start<<" "<<start+segLen<<endl;
        //cout<<start<<" "<<start+segLen<<endl;
        if(++i > 50){
            break;
        }
    }
    output.close();
//
//    
    
    //waitKey(0);
    return 0;
}



bool proccess(vector<pair<int, int> > )
