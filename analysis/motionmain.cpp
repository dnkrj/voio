#include <iostream>
#include <cstdlib>
#include <string>

#include "MotionAnalyzer.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 4) {
		std::cout << "Usage: motionmain <filename> <uid> <gifpathprefix>" << std::endl;
		return 1;
	}
	try {
		Filter f;
		MotionAnalyzer ls;
		std::vector<Timestamp> p = ls.processVideo(std::string(argv[1]), 3); 
		f.extractGifs(std::string(argv[1]), std::string(argv[3]), atoi(argv[2]), p);
	} catch(std::string s) {
		std::cout << s << std::endl;
	}
	return 0;
}
