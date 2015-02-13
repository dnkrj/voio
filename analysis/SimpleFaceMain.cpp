#include <iostream>
#include <cstdlib>
#include <string>

#include "SimpleFaceStrategy.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		std::cout << "Usage: main <filename> <secondsPerClip>" << std::endl;
		return 1;
	}
	try {
		Filter f;
		SimpleFaceStrategy ls;
		std::vector<double> timestamps = ls.processVideo(std::string(argv[1]), atoi(argv[2])); 
		std::vector<Timestamp> p(timestamps.size()/2);
		for(unsigned int i = 0; i<timestamps.size(); i += 2) {
			Timestamp t(timestamps[i], timestamps[i+1]);
			p[i/2] = t;
		}
		std::vector<GIF> gifs = f.extractGifs(std::string(argv[1]), 001100, p);
	} catch(std::string s) {
		std::cout << s << std::endl;
	}
	return 0;
}
