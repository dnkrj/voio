#include "SimpleFaceStrategy.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

//Configurations for strategy
//Config for face detection
#define minNeighbours 5
#define scalingPerRun 1.1

//Config for sampling
#define samplesPerSecond 4

using namespace cv;
void skipFrames(VideoCapture & vid, int numFramesToSkip)
{
	for (int i = 0; i < numFramesToSkip; i++)
	{
		vid.grab();
	}
}

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


std::vector<double> SimpleFaceStrategy::processVideo(const std::string & filename, int secondsPerClip)
{
	std::vector<double> timestamps;
	VideoCapture readAhead(filename);
	

	long frameCount = readAhead.get(CAP_PROP_FRAME_COUNT);
	double fps = readAhead.get(CAP_PROP_FPS);
	int framesPerSample = (fps / samplesPerSecond); //Sample 4 frames a second
	int numFramesToSkip = framesPerSample - 1;
	int numWindows = frameCount / framesPerSample; //Actual value will be smaller than this
	std::cout << "Num windows : " << numWindows << "\n";
	
	int windowSize = fps*secondsPerClip; //Number of frames in a given window.
	windowSize += windowSize % framesPerSample; //Make window size divisible by sample rate
	int samplesPerWindow = windowSize / framesPerSample;
	
	std::string face_cascade_name ="haarcascade_frontalface_alt.xml";
	std::vector<Rect> detectedFaces;
	CascadeClassifier faceCascade;
	faceCascade.load(face_cascade_name);

	String s = "outvid_simple_face.avi";
	Size size = Size(1280, 720);
	namedWindow("test");
	VideoWriter writer;
	writer.open(s, -1, fps, size);
	if (writer.isOpened())
	{
		std::cout << "opened";
	}

	Mat frame;
	int * facesInSample = new int[samplesPerWindow];
	int * windowValues = new int[numWindows];
	windowValues[0] = 0;
	int windowIndex = 0;
	int facesIndex = 0;
	for (int i = 0; i < samplesPerWindow; i++)
	{
		readAhead.read(frame);
		faceCascade.detectMultiScale(frame, detectedFaces, scalingPerRun, minNeighbours);
		facesInSample[i] = detectedFaces.size();
		windowValues[0] += facesInSample[i];
		for (Rect r : detectedFaces)
		{
			rectangle(frame, r, Scalar(255, 255, 255));
		}
		writer.write(frame);
		skipFrames(readAhead, numFramesToSkip);
	}
	windowIndex = 1;

	while (readAhead.read(frame))
	{
		//Detect faces
		faceCascade.detectMultiScale(frame, detectedFaces, scalingPerRun, minNeighbours);
		//Update window values
		windowValues[windowIndex] = windowValues[windowIndex - 1];
		windowValues[windowIndex] += (detectedFaces.size() - facesInSample[facesIndex]);
		//Update sampleBuffer
		facesInSample[facesIndex] = detectedFaces.size();
		//Update indexes
		facesIndex = (facesIndex + 1) % samplesPerWindow;
		++windowIndex;
		//Skip frames to next sample
		skipFrames(readAhead, numFramesToSkip);
		std::cout << "Processed window: " <<windowIndex-1 <<  "\n";
	}

	//Cleanup any non-set windows
	while (windowIndex < numWindows)
	{
		windowValues[windowIndex] = 0;
		windowIndex++;
	}
	

	int maxIndex = 0;
	for (int x = 0; x < 10; x++)
	{
		maxIndex = 0;
		for (int t = 0; t <windowSize; t++)
		{
			if (windowValues[t] > windowValues[maxIndex])
			{
				maxIndex = t;
			}
		}

		double endFrame = (maxIndex + 1) * windowSize;
		double startFrame = endFrame - windowSize;

		double endTime = endFrame / fps;
		double startTime = startFrame / fps;

		timestamps.push_back(startTime);
		timestamps.push_back(endTime);
		timestamps.push_back(windowValues[maxIndex]);
		windowValues[maxIndex] = 0;
	}
	writer.release();
	delete[] facesInSample;
	delete[] windowValues;

	return timestamps;
}
