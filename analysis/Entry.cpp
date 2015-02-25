#include <iostream>
#include <vector>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "gifs/gifcv.h"
#include "LazyStrategy.h"
#include "SimpleFaceStrategy.h"

#define GifLength 3

using namespace cv;

int main(int argc, char ** argv)
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
	
	int time = frameCount / fps;
	int numGifs;
	if (time < GifLength * 4) 
	{
		numGifs = time/GifLength;
	}
	else
	{
		numGifs = (time % 60) + 4;
	}
	//Run through our strategies
	
	std::vector<Timestamp> simpleFaceTimestamps;
	SimpleFaceStrategy simpleFace;
	simpleFaceTimestamps = simpleFace.processVideo(filename, GifLength);
	
	std::cout << "SimpleFace - found : " << simpleFaceTimestamps.size() << std::endl;
	for (int i = 0; i < simpleFaceTimestamps.size(); i++)
	{
		timestamps.push_back(simpleFaceTimestamps.at(i));
	}

	while (timestamps.size() < numGifs)
	{
		LazyStrategy ls;
		std::vector<Timestamp> lazyTimestamps;
		lazyTimestamps = ls.processVideo(filename, GifLength);
		int missingGIFs = numGifs - timestamps.size();
		int index = 0;
		while (missingGIFs && lazyTimestamps.size())
		{
			timestamps.push_back(lazyTimestamps.at(0));
			lazyTimestamps.erase(lazyTimestamps.begin());
			numGifs++;
		}
	}

	std::cout << "TS generated starting GIF(TS:) " << timestamps.size()  <<  std::endl;

	Filter filter;
	filter.extractGifs(filename, outputdir, 0, timestamps);

	std::cout << "Finished successfully" << std::endl;	
	
	return 0;
	
}	