#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "SimpleFaceStrategy.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		std::cout << "Usage: main <filename> <pathprefix>" << std::endl;
		return 1;
	}
	try {
		std::vector<std::string> sp1 = split(std::string(argv[2]), '/');
		Filter f;
		SimpleFaceStrategy ls;

		std::cout << "Starting analysis" << std::endl;
		std::vector<Timestamp> timestamps = ls.processVideo(std::string(argv[1]), 3); 
		std::cout << "Starting gif production with : " << timestamps.size() << std::endl;
		f.extractGifs(std::string(argv[1]), std::string(argv[2]), 0, timestamps);
		
		//Will move network confirmation to Entry.cpp
		//confirm(sp[sp.size()-2].c_str());
	} catch(const char* s) {
		std::cout << "Error caught in catch" << std::endl;
		std::cout << std::string(s) << std::endl;
	}
	return 0;
}
