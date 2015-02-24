#include <iostream>
#include <cstdlib>
#include <string>

#include "SimpleFaceStrategy.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		std::cout << "Usage: main <filename> <pathprefix>" << std::endl;
		return 1;
	}
	try {
		Filter f;
		SimpleFaceStrategy ls;

		std::cout << "Starting analysis" < std::endl;
		std::vector<Timestamp> timestamps = ls.processVideo(std::string(argv[1]), 3); 
		std::cout << "Starting gif production with : " << timestamps.size() << std::endl;
		f.extractGifs(std::string(argv[1]), std::string(argv[2]), 0, timestamps);
	} catch(std::string s) {
		std::cout << s << std::endl;
	}
	return 0;
}
