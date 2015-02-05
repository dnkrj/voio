#include <iostream>
#include <string>

#include "LazyStrategy.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		cout << "Usage: main <filename> <secondsPerClip>" << endl;
		return 1;
	}
	try {
		Filter f;
		LazyStrategy ls;
		vector<double> timestamps = ls.processVideo(string(argv[1]), atoi(argv[2])); 
		vector<Timestamp> p(timestamps.size());
		for(int i = 0; i<timestamps.size(); i += 2) {
			Timestamp t(timestamps[i], timestamps[i+1]);
			p[i] = t;
		}
		vector<GIF> gifs = f.extractGifs(string(argv[1]), 24915, p);
	} catch(string s) {
		cout << s << endl;
	}
	return 0;
}
