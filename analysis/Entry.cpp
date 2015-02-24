#include <iostream>
#include <vector>
#include <string>
#include "opencv2/core/core"
#include "opencv2/highgui/highgui.cpp"
#include "gifs/gifcv.h"
#include "LazyStrategy.h"
#include "SimpleFaceStrategy.h"

#define GifLength 5

int main(int argc, char ** argc)
{
	if (argc != 3)
	{
		std::cout << "Usage : this <VideoName> <OutputPath> " << std::endl;
	}
	

	std::vector<Timestamp> timestamps;
	std::string filename = std::string(argv[1]);
	std::string outputdir = std::string(argv[2]);

	VideoCapture vidInfo(filename);
	long frameCount = vidInfo.get(CV_CAP_PROP_FRAME_COUNT);
	double fps = vidInfo.get(CV_CAP_PROP_FPS);
	
	double time = frameCount / fps;
	int numGifs = (time / 60) + 1;

	//Run through our strategies
	
	std::vector<Timestamp> simpleFaceTimestamps;
	SimpleFaceStrategy simpleFace;
	simpleFaceTimestamps = simpleFace.processVideo(filename, GifLength);
	
	for (int i = 0; i < simpeFaceTimestamps.size(); i++)
	{
		timestamps.add(simpleFaceTimestamps.at(i));
	}

	if (timestamps.size() < numGifs)
	{
		LazyStrategy ls;
		std::vector<Timestamp> lazyTimestamps;
		lazyTimestamps = ls.processVideo(filename, GifLength);
		int missingGIFs = numGifs - timestamps.size();
		int numTS = lazyTimestamps.size();
		for (int i = 0; i < numTS; i++)
		{
				
	}

	//Filter
	
	//Run through gif generation	

	
	
	
	
