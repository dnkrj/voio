#ifndef _PROBE_FPS
#define _PROBE_FPS

#include <stdio.h>
#include <iostream>
#include <string>

double findFPS(const std::string& input) {
	std::string m("fps");
	double fps = 24;
	// different member versions of find in the same order as above:
	std::size_t found = input.find(m);
	if (found != std::string::npos) {
		//std::cout << "first 'fps' found at: " << found << std::endl;
		std::string rstrct = input.substr(found - 8, 8);
		//std::cout << rstrct << std::endl;
		std::string tc = "";
		for(unsigned int i = 0; i<rstrct.size(); i++) {
			if(i != 0 && i != 1 && ((rstrct[i] >= '0' && rstrct[i] <= '9') || rstrct[i] == '.')) {
				tc += rstrct[i];
			}
		}
		//std::cout << tc << std::endl;
		fps = std::stod(tc);
	}
	return fps;
}

double getFPS(const std::string& filename) {
	FILE *in;
	char buff[512];
	std::string command = "avprobe " + filename + " 2>&1";
	std::string output = "";

	if(!(in = popen(command.c_str(), "r"))){
		exit(1);
	}
	
	while(fgets(buff, sizeof(buff), in) != 0){
		output += std::string(buff);
	}
	//std::cout << output;// << std::endl;
	pclose(in);
	return findFPS(output);
}
#endif
