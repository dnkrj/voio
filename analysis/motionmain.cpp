#include <iostream>
#include <cstdlib>
#include <string>

#include "MotionAnalyzer.h"
#include "gifs/timestamp.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		if(argc == 1) {
			MotionAnalyzer demo;
			try {
				demo.runDemo(0);
			} catch(const char* s) {
				std::cerr << std::string(s) << std::endl;
			}
		} else if (argc == 2) {
			MotionAnalyzer demo;
			try {
				demo.runDemo(std::string(argv[1]));
			} catch(const char* s) {
				std::cerr << std::string(s) << std::endl;
			}
		} else {
			std::cout << "Analysis usage: motionmain <filename> <pathprefix>" << std::endl;
			std::cout << "Demo usage: motionmain" << std::endl;
			return 1;
		}
	} else {
		try {
			Filter f;
			MotionAnalyzer ls;
			std::vector<Timestamp> p = ls.processVideo(std::string(argv[1]), 3); 
			f.extractVids(std::string(argv[1]), std::string(argv[2]), 0, p);
		} catch(const char* s) {
			std::cerr << std::string(s) << std::endl;
		}
	}
	return 0;
}
