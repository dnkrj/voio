#include "SimpleFaceStrategy.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "gifs/gifcv.h"
#include <iostream>

//Configurations for strategy
//Config for face detection
#define minNeighbours 3 
#define scalingPerRun 1.2

//Config for sampling
#define samplesPerSecond 5 


/*
This algorithm approaches the problem in the following way:

Firstly, we consider a window to be a sub-section of the video, equal in length
to the number of seconds requested in a GIF

For each window, we calculate the total number of faces found in each sample, in 
the entire window - this is stored in windowValues
A sample is taken so many times a second.

In order to calculate this, we keep a buffer "facesInSample" which stores 
the number of faces in each sample, for last window. 

Once we have our value for each window, we sort it.
Next we choose a number of windows (those with the highest value associated with it)
*/

struct Window
{
	int numFaces;
	int index;
};

bool sortFunc(Window l, Window r) { return l.numFaces > r.numFaces; }


using namespace cv;
void skipFrames(VideoCapture & vid, int numFramesToSkip)
{
	for (int i = 0; i < numFramesToSkip; i++)
	{
		vid.grab();
	}
}

std::vector<Timestamp> SimpleFaceStrategy::processVideo(const std::string & filename, int secondsPerClip)
{
	std::vector<Timestamp> timestamps;
	VideoCapture readAhead(filename);

	long frameCount = readAhead.get(CV_CAP_PROP_FRAME_COUNT);
	double fps = getFPS(filename);
	int framesPerSample = (fps / samplesPerSecond); //Sample 4 frames a second
	int numFramesToSkip = framesPerSample - 1;
	int numWindows = frameCount / framesPerSample; //Actual value will be smaller than this
	std::cout << "Num windows : " << numWindows << "\n";
	
	int windowSize = fps*secondsPerClip; //Number of frames in a given window.
	windowSize += windowSize % framesPerSample; //Make window size divisible by sample rate
	int samplesPerWindow = windowSize / framesPerSample;
	
	std::string face_cascade_name ="/home/voio/VideoDiary/AnalysisOutput/haarcascade_frontalface_alt.xml";
	std::vector<Rect> detectedFaces;
	CascadeClassifier faceCascade;
	faceCascade.load(face_cascade_name);
	
	Mat frame;
	Mat greyFrame;
	int * facesInSample = new int[samplesPerWindow];
	Window * Windows  = new Window[numWindows];
	Windows[0].index = 0;
	Windows[0].numFaces = 0;
	int windowIndex = 0;
	int facesIndex = 0;
	for (int i = 0; i < samplesPerWindow; i++)
	{
		readAhead.read(frame);
		if(frame.empty()) return timestamps;
		cvtColor(frame, greyFrame, COLOR_BGR2GRAY);
		if(greyFrame.empty()) return timestamps;
		faceCascade.detectMultiScale(greyFrame, detectedFaces, scalingPerRun, minNeighbours);
		facesInSample[i] = detectedFaces.size();
		Windows[0].numFaces += facesInSample[i];
		skipFrames(readAhead, numFramesToSkip);
	}
	windowIndex = 1;

	while (readAhead.read(frame))
	{
		//Detect faces
		faceCascade.detectMultiScale(frame, detectedFaces, scalingPerRun, minNeighbours);
		//Update window values
		Windows[windowIndex].numFaces = Windows[windowIndex - 1].numFaces;
		Windows[windowIndex].numFaces += (detectedFaces.size() - facesInSample[facesIndex]);
		Windows[windowIndex].index = windowIndex;
		//Update sampleBuffer
		
		facesInSample[facesIndex] = detectedFaces.size();
		//Update indexes
		facesIndex = (facesIndex + 1) % samplesPerWindow;
		++windowIndex;
		//Skip frames to next sample
		skipFrames(readAhead, numFramesToSkip);
		if (!(windowIndex % 200))
		{
			std::cout << "Processed window: " << windowIndex << std::endl;
		}
	}

	//Cleanup any non-set windows
	while (windowIndex < numWindows)
	{
		Windows[windowIndex].numFaces = 0;
		Windows[windowIndex].index = windowIndex;
		windowIndex++;
	}
	
	std::sort(Windows, &Windows[numWindows], sortFunc);
	int numGIFs;
	int time = frameCount / fps;
	if (frameCount / fps < secondsPerClip*4)
	{
		numGIFs = (time) / secondsPerClip;
	}
	else
	{
		numGIFs = ((time) / 60) + 4;
	}
	
	int secondsBetweenWindow = 5;
	double indexGap = samplesPerSecond * secondsBetweenWindow;
	std::cout << Windows[0].numFaces << std::endl;
	std::cout << Windows[numWindows - 1].numFaces << std::endl;
	std::cout << "Starting filter and sort" << std::endl;
	for (int i = 0; i < numWindows; i++)
	{
		for (int x = i+1; x < numWindows; x++)
		{
			if (abs(Windows[i].index - Windows[x].index) < indexGap)
			{
				if (Windows[i].numFaces != 0)
				{
					//std::cout << "Deleting index " << x << std::endl;
					Windows[x].numFaces = 0;
				}
			}
		}
	}
	int added = 0;
	for (int i = 0; i < numWindows; i++)
	{
		if (Windows[i].numFaces != 0 || added == 0)
		{
			added++;
			double endFrame = windowSize + (framesPerSample * Windows[i].index);
			double startFrame = endFrame - windowSize;

			double endTime = endFrame / fps;
			double startTime = startFrame / fps;
			timestamps.push_back(Timestamp(startTime*1000, endTime*1000));
		}
		if (added == numGIFs)
		{
			break;
		}
	}

	delete[] facesInSample;
	delete[] Windows;

	return timestamps;
}
