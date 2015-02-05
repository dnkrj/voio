#include "LazyStrategy.h"
#ifndef _OPEN_CV
#define 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#endif

using namespace cv;

std::vector<double>  LazyStrategy::processVideo(const std::string & filename, int secondsPerClip)
{
	VideoCapture vidReader(filename);
	long frameCount = vidReader.get(CAP_PROP_FRAME_COUNT);
	double fps = vidReader.get(CAP_PROP_FPS);

	std::vector<double> timestamps;
	
	long totalTimeSeconds = frameCount / fps;
	int totalTimeMinutes = totalTimeSeconds / 60;
	
	if (totalTimeMinutes == 0)
	{
		if (totalTimeSeconds > secondsPerClip)
		{
			timestamps.push_back(0.0);
			timestamps.push_back(secondsPerClip);
		}
		return timestamps;
	}
	
	for (int i = 0; i < totalTimeMinutes; i++)
	{
		int randomTime = rand() % (60-secondsPerClip);
		randomTime += i * 60;
		timestamps.push_back(randomTime);
		timestamps.push_back(randomTime + secondsPerClip);
	}

	return timestamps;
}