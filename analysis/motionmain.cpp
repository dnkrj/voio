#include <iostream>
#include <cstdlib>
#include <string>

#include "MotionAnalyzer.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 4) {
		std::cout << "Usage: motionmain <filename> <uid> <seconds per clip>" << std::endl;
		return 1;
	}
	try {
		Filter f;
		MotionAnalyzer ls;
		std::vector<Timestamp> p = ls.processVideo(std::string(argv[1]), atoi(argv[3])); 
		std::vector<GIF> gifs = f.extractGifs(std::string(argv[1]), atoi(argv[2]), p);
	} catch(std::string s) {
		std::cout << s << std::endl;
	}
	return 0;
}
