#ifndef _GIF_CV
#define _GIF_CV

#ifndef _GIFLIB
#define _GIFLIB
#include <gif_lib.h>
#endif

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

#ifndef _OPEN_CV
#define _OPEN_CV
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#endif

using namespace cv;

typedef std::vector<GifByteType> frame_t;

#include "gifcv.h"

//Utility functions.
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s, ios_base::in | ios_base::out);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}


//GIF member functions.
GIF::GIF(void) {}
GIF::~GIF(void) {}
int GIF::getUID(void) {return userID;}
int GIF::getGID(void) {return gifID;}
bool GIF::isPublished(void) {return published;}
std::string GIF::publish(void) {
	published = true;
	return gifPath;
}

void GIF::create(int uid, int gid, std::string vP) {
	userID = uid;
	gifID = gid;
	videoPath = vP;
	published = false;
	std::vector<std::string> sp = split(videoPath, '.');
	gifPath = sp.at(0) + to_std::string(uid) + to_std::string(gid) + ".gif";
}

std::string GIF::getPath(void) {
	return gifPath;
}

std::string GIF::getVideoPath(void) {
	std::vector<std::string> sp = split(videoPath, '.');
	return sp.at(0) + to_std::string(gifID) + ".avi";
}


//VideoConverter member functions.
VideoConverter::VideoConverter(int sx, int sy) {
	gid = 0;
	gifsx = sx;
	gifsy = sy;
	paletteSize = 256;
	outputPalette = GifMakeMapObject(paletteSize, NULL);
}

VideoConverter::~VideoConverter(void) {
  	frames.clear();
  	delay.clear();
}

bool VideoConverter::clear(void) {
	frames.clear();
	delay.clear();
	return true;
}

bool VideoConverter::reset(void) {
	gid = 0;
	frames.clear();
	delay.clear();
	return true;
}

bool VideoConverter::addFrame(uint8_t* data, float dt){
	int N = gifsx*gifsy;
  	
	frame_t output(N);
  
  	if(frames.size() == 0) {
  
		frame_t r(N),g(N),b(N);

		for(int i=0, j=0; i<N; i++) {
			b[i] = data[j++];
			g[i] = data[j++];
			r[i] = data[j++];
		}

		if(GifQuantizeBuffer(gifsx, gifsy, &paletteSize, &(r[0]),&(g[0]),&(b[0]), &(output[0]), outputPalette->Colors) == GIF_ERROR) return false;

	} else {
		for(int i = 0, j=0; i < N; i++) {
       		
			int minIndex = 0, minDist = 3 * 256 * 256;
			GifColorType *c = outputPalette->Colors;
	
			for(int k = 0; k < outputPalette->ColorCount; k++) {
				int db = (int(c[k].Blue) - int(data[j]));
				int dg = (int(c[k].Green) - int(data[j+1]));
				int dr = (int(c[k].Red) - int(data[j+2]));
				int dist=dr*dr+dg*dg+db*db;
          
				if (dist < minDist) {
					minDist  = dist;
					minIndex = k;
				}
			}
        
			j+=3;
			output[i] = minIndex;
		}	
	}
  
	frames.push_back(output);
	delay.push_back(int(dt*100.0));
	return true;       
}

GIF VideoConverter::extractGif(const std::string& src, int uid, double start, double end) {
	if(!outputPalette) throw "Error creating colour map.";
	GIF gif;
	gif.create(uid, gid, src);
    
	if(!cap.open(src)) throw "Error opening file.";
	else {
        	Mat frame;
        	Mat frame_c;
        	Mat frame_r;
        	Mat frame_n;
        	double fps = cap.get(CV_CAP_PROP_FPS);
        	float rate = (float) (1/fps);
        	double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
        	double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
        	cap.set(CV_CAP_PROP_CONVERT_RGB, double(true));
        	cap.set(CV_CAP_PROP_POS_MSEC, start);
        	double ratio = width/height;
        	
        	while(cap.get(CV_CAP_PROP_POS_MSEC)<end) {
        		if(!cap.read(frame)) throw "Error reading frames.";
        		if(ratio < 1) {
         			getRectSubPix(frame, Size((int) width, (int) width), Point2f((float) width/2, (float) height/2), frame_c, -1);
	       		} else if(ratio > 1) {
        	        	getRectSubPix(frame, Size((int) height, (int) height), Point2f((float) width/2, (float) height/2), frame_c, -1);
        		} else {
        			frame_c = frame;
        		}
			resize(frame_c, frame_r, Size(gifsx, gifsy), 1.0, 1.0, INTER_LINEAR);
			frame_r.convertTo(frame_n, CV_8UC3, 1.0, 0);
			if(frame_r.isContinuous()) {
				if(!addFrame(frame_n.data, rate)) throw "Error writing to file.";
			}
		}	
	}

	if(!save(gif.getPath().c_str())) throw "Error writing to file.";
	gid++;
	if(!clear()) throw "Error clearing frames.";
	return gif;
}

bool VideoConverter::addLoop(GifFileType *gf) {
	int loop_count = 0;
	char nsle[12] = "NETSCAPE2.0";
	char subblock[3];
	if(EGifPutExtensionLeader(gf, APPLICATION_EXT_FUNC_CODE) == GIF_ERROR) {
		return false;
	}
	if(EGifPutExtensionBlock(gf, 11, nsle) == GIF_ERROR) return false;
	subblock[0] = 1;
	subblock[2] = loop_count % 256;
	subblock[1] = loop_count / 256;
	if(EGifPutExtensionBlock(gf, 3, subblock) == GIF_ERROR) return false;
	if(EGifPutExtensionTrailer(gf) == GIF_ERROR) {
		return false;
	}
	return true;
}

bool VideoConverter::save(const char* filename) {
	if(frames.size() == 0) return false;
  
 	GifFileType *GifFile = EGifOpenFileName(filename, false, NULL);
  
  	if(!GifFile) return false;
	if(EGifPutScreenDesc(GifFile, gifsx, gifsy, 8, 0, outputPalette) == GIF_ERROR) return false;

  	if(!addLoop(GifFile)) return false;

  	for(int ni=0; ni< int(frames.size()); ni++) {      

    	static uint8_t ExtStr[4] = { 0x04, 0x00, 0x00, 0xff };
    
    	ExtStr[0] = (false) ? 0x06 : 0x04;
    	ExtStr[1] = delay[ni] % 256;
    	ExtStr[2] = delay[ni] / 256;

    	EGifPutExtension(GifFile, GRAPHICS_EXT_FUNC_CODE, 4, ExtStr);    
    
    	if(EGifPutImageDesc(GifFile, 0, 0, gifsx, gifsy, false, NULL) == GIF_ERROR) return false;
       
       
    	for(int j = 0; j<gifsy; j++) {
    			if(EGifPutLine(GifFile, &(frames[ni][j*gifsx]), gifsx) == GIF_ERROR) return false;
 		}
  	}

  	if(EGifCloseFile(GifFile, NULL) == GIF_ERROR) return false;

  	return true;       
}

//Timestamp member functions.
Timestamp::Timestamp(double a, double b) : start(a), end(b) {}
Timestamp::Timestamp(void) {}
void Timestamp::create(double a, double b) {
	start = a;
	end = b;
}
Timestamp::~Timestamp(void) {}
double Timestamp::getStart(void) {return start;}
double Timestamp::getEnd(void) {return end;}

//Filter member functions.
Filter::Filter(void) : vc(300, 300) {
}
Filter::~Filter(void) {}
std::vector<GIF> Filter::extractGifs(const std::string& filename, int uid, std::vector<Timestamp>& ts) {
	int size = int(ts.size());
	std::vector<GIF> gifs;
	if(size>MAX) {
		gifs = std::vector<GIF>(MAX);
		double x = (double) (MAX);
		double y = (double) (size);
		double z = y/x;
		int u = 0;
		x = 0;
		while(u<size) {
			gifs[u] = vc.extractGif(filename, uid, ts[u].getStart(), ts[u].getEnd());
			x += z;
			u = int(x);
		}
	} else {
		gifs = std::vector<GIF>(size);
		for(int i = 0; i<size; i++) {
			gifs[i] = vc.extractGif(filename, uid, ts[i].getStart(), ts[i].getEnd());
		}
	}
	if(!vc.reset()) throw "Error clearing frames.";
	return gifs;
}

//Test program.
int main(int argc, char** argv) {
	try {
		if(argc != 3) {
			cout << "Usage: gifcv <filename> <uid>" << endl;
			return 1;
		}
		Filter f;
		std::vector<Timestamp> ts(4);
		Timestamp x1(0, 500);
		Timestamp x2(500, 1000);
		Timestamp x3(1000, 1500);
		Timestamp x4(1500, 2000);
		ts[0] = x1;
		ts[1] = x2;
		ts[2] = x3;
		ts[3] = x4;
		std::vector<GIF> vs = f.extractGifs(std::string(argv[1]), atoi(argv[2]), ts); 
	} catch(std::string s) {
		cout << s << endl;
		return 2;
	}
	return 0;
}

#endif
