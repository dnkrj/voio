#ifndef _GIFLIB
#define _GIFLIB
#include <gif_lib.h>
#endif

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#ifndef _OPEN_CV
#define _OPEN_CV
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/video.hpp"
#endif

#include "gifcv.h"
#include "timestamp.h"
#include "fps.c"

using namespace cv;

//Utility functions.
std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s, std::ios_base::in | std::ios_base::out);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

//VideoConverter member functions.
std::string VideoConverter::getPath(int uid, int gid, std::string vP, const std::string& prepath) {
	std::vector<std::string> sp1 = split(vP, '/');
	std::vector<std::string> sp = split(sp1[sp1.size()-1], '.');
	return prepath + sp.at(0) + std::to_string(uid) + std::to_string(gid) + ".gif";
}

std::string VideoConverter::getVideoPath(int uid, int vid, std::string vP, const std::string& prepath) {
	std::vector<std::string> sp1 = split(vP, '/');
	std::vector<std::string> sp = split(sp1[sp1.size()-1], '.');
	return prepath + "TMP" + sp.at(0) + std::to_string(uid) + std::to_string(vid) + ".avi";
}

std::string VideoConverter::getFinalPath(int uid, int vid, std::string vP, const std::string& prepath) {
	std::vector<std::string> sp1 = split(vP, '/');
	std::vector<std::string> sp = split(sp1[sp1.size()-1], '.');
	return prepath + sp.at(0) + std::to_string(uid) + std::to_string(vid);
}

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

bool VideoConverter::addFrame(uint8_t* data, float dt) {
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

void VideoConverter::extractGif(const std::string& src, const std::string& path, int uid, double start, double end) {
	if(!outputPalette) throw "Error creating colour map.";
	std::cout << "Saving GIF between " << start << " and " << end << std::endl;
	if(!cap.open(src)) throw "Error opening file.";
	else {
        Mat frame;
        Mat frame_c;
        Mat frame_r;
        Mat frame_n;
        double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
        double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
        cap.set(CV_CAP_PROP_CONVERT_RGB, double(true));
        cap.set(CV_CAP_PROP_POS_MSEC, start);
        double ratio = width/height;
		//double pt = cap.get(CV_CAP_PROP_POS_MSEC);
		double fps = getFPS(src);
		std::cout << "FPS: " << fps << std::endl;
		float dt = float(1/fps);
        
        while(cap.get(CV_CAP_PROP_POS_MSEC)<end) {
        	//std::cout << "Current fps: " << 1000/dt << std::endl;
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
			if(frame_n.isContinuous()) {
				if(!addFrame(frame_n.data, dt)) throw "Error writing to file.";
			}
			//dt = cap.get(CV_CAP_PROP_POS_MSEC) - pt;
			//pt = cap.get(CV_CAP_PROP_POS_MSEC);
		}	
	}

	if(!save(getPath(uid, gid, src, path).c_str())) throw "Error writing to file.";
	gid++;
	if(!clear()) throw "Error clearing frames.";
}

/*
void VideoConverter::extractVid(const std::string& src, const std::string& path, int uid, double start, double end) {
	std::string vp = getVideoPath(uid, gid, src, path);
	std::cout << "Saving video between " << start << " and " << end << std::endl;
	double fps = getFPS(src);
	std::cout << "FPS: " << fps << std::endl;
	if(!cap.open(src)) throw "Error opening file.";
	else {
		cap.set(CV_CAP_PROP_POS_AVI_RATIO, 1);
		double length = cap.get(CV_CAP_PROP_POS_MSEC);
		if(end>length || start<0) return;
		cap.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
		Mat temp;
		cap.set(CV_CAP_PROP_POS_MSEC, start);
		double t0 = cap.get(CV_CAP_PROP_POS_MSEC);
		if(!cap.read(temp)) throw "Error reading frames.";
		double dt = cap.get(CV_CAP_PROP_POS_MSEC) - t0;
		double fps = 1000/dt;
		std::cout << "Current fps: " << fps << std::endl;
		Mat frame;
		Mat frame_c;
		Mat frame_r;
		Mat frame_n;
		Mat frame_f;
		double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		cap.set(CV_CAP_PROP_POS_MSEC, start);
		double ratio = width/height;
		VideoWriter video(vp, CV_FOURCC('M','J','P','G'), fps, Size(gifsx, gifsy), true);
        	
		while(cap.get(CV_CAP_PROP_POS_MSEC)<end) {
			if(!cap.read(frame)) break;
			if(ratio < 1) {
				getRectSubPix(frame, Size((int) width, (int) width), Point2f((float) width/2, (float) height/2), frame_c, -1);
			} else if(ratio > 1) {
				getRectSubPix(frame, Size((int) height, (int) height), Point2f((float) width/2, (float) height/2), frame_c, -1);
			} else {
				frame_c = frame;
			}
			resize(frame_c, frame_r, Size(gifsx, gifsy), 1.0, 1.0, INTER_CUBIC);
			video.write(frame_r);
		}	
	}
	std::string cmd = "/usr/bin/avconv -y -i " + vp + " -vcodec libx264 " + getFinalPath(uid, gid, src, path) + ".mp4";
	system(cmd.c_str());
	//std::string cmd2 = "/usr/bin/avconv -y -i " + vp + " -vcodec libvpx " + getFinalPath(uid, gid, src, path) + ".webm > /dev/null 2>&1";
	//system(cmd2.c_str());
	if(remove(vp.c_str()) != 0) throw "Could not delete temporary AVI file.";
	gid++;
}*/

void VideoConverter::extractVid(const std::string& src, const std::string& path, int uid, double start, double end) {

	std::cout << "Saving video between " << start << " and " << end << std::endl;
	if(!cap.open(src)) throw "Error opening file.";
	else {
		double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		double ratio = width/height;
		int x;
		int y;
		double size;
		if(ratio < 1) {
			x = 0;
			y = (int) ((height - width)/2);
			size = width;
		} else if(ratio > 1) {
			x = (int) ((width - height)/2);
			y = 0;
			size = height;
		} else {
			x = 0;
			y = 0;
			size = width;
		}
		
		std::string sx = std::string(std::to_string(x));
		std::string sy = std::string(std::to_string(y));
		std::string strw = std::string(std::to_string((int) size));
		std::string strh = std::string(std::to_string((int) size));
		
		int sm = (int) (start/60000);
		int sh = sm/60;
		sm = sm%60;
		double sst1 = (double) (((long) start) % 60000);
		double sst2 = sst1/1000;
		int ss = (int) sst2;
		double ssst = sst2 - ((double) ss);
		int sss = (int) (ssst*1000);
		
		int em = (int) ((end-start)/60000);
		int eh = em/60;
		em = em%60;
		double est1 = (double) (((long) (end-start))%60000);
		double est2 = est1/1000;
		int es = (int) est2;
		double esst = est2 - ((double) es);
		int ess = (int) (esst*1000);
		
		std::string starth;
		std::string startm;
		std::string starts;
		std::string startd;
		if(sh<10) starth = "0" + std::string(std::to_string(sh));
		else starth = std::string(std::to_string(sh));
		if(sm<10) startm = "0" + std::string(std::to_string(sm));
		else startm = std::string(std::to_string(sm));
		if(ss<10) starts = "0" + std::string(std::to_string(ss));
		else starts = std::string(std::to_string(ss));
		if(sss<10) startd = ".00" + std::string(std::to_string(sss));
		else if(sss<100) startd = ".0" + std::string(std::to_string(sss));
		else startd = "." + std::string(std::to_string(sss));
		
		std::string endh;
		std::string endm;
		std::string ends;
		std::string endd;
		if(eh<10) endh = "0" + std::string(std::to_string(eh));
		else endh = std::string(std::to_string(eh));
		if(em<10) endm = "0" + std::string(std::to_string(em));
		else endm = std::string(std::to_string(em));
		if(es<10) ends = "0" + std::string(std::to_string(es));
		else ends = std::string(std::to_string(es));
		if(ess<10) endd = ".00" + std::string(std::to_string(ess));
		else if(ess<100) endd = ".0" + std::string(std::to_string(ess));
		else endd = "." + std::string(std::to_string(ess));
		
		std::string cmd = "/usr/bin/avconv -y -i " + src
		+ " -ss " + starth + ":" + startm + ":" + starts + startd
		+ " -t " + endh + ":" + endm + ":" + ends + endd
		+ " -vf crop=" + strw + ":" + strh + ":" + sx + ":" + sy
		+ " -acodec copy -vcodec libx264 "
		+ " -s 300x300 "
		+ getFinalPath(uid, gid, src, path) + ".mp4 > /dev/null 2>&1";
		//std::cout << cmd << std::endl;
		system(cmd.c_str());
		gid++;
	}
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
	if(frames.size() == 0) throw "Number of frames is zero.";

 	GifFileType *GifFile = EGifOpenFileName(filename, false, NULL);
  
  	if(!GifFile) return false;
	if(EGifPutScreenDesc(GifFile, gifsx, gifsy, 8, 0, outputPalette) == GIF_ERROR) return false;

  	if(!addLoop(GifFile)) return false;

  	for(int ni=0; ni<int(frames.size()); ni++) {      

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

//Filter member functions.
Filter::Filter(void) : vc(300, 300) {}
Filter::~Filter(void) {}
static bool overlapsTooMuch(Timestamp t1, Timestamp t2) {
	double Astart = t1.getStart();
	double Aend = t1.getEnd();
	double Bstart = t2.getStart();
	double Bend = t2.getEnd();
	double delta = Aend - Astart;
	if(Aend <= Bstart || Bend <= Astart) return false;
	else {
		if((Astart >= Bstart && Aend <= Bend) || (Bstart >= Astart && Bend <= Aend)) {
			return true;
		} else if(Astart > Bstart) {
			return ((Bend - Astart)/delta) > 0.4;
		} else if(Bstart > Astart) {
			return ((Aend - Bstart)/delta) > 0.4;
		}
	}
	return false;
}
void Filter::extractGifs(const std::string& filename, const std::string& path, int uid, std::vector<Timestamp>& is) {
	std::vector<Timestamp> ts;
	for(auto t : is) {
		bool safe = true;
		for(auto test : ts) {
			if(overlapsTooMuch(t, test)) {
				safe = false;
				break;
			}
		}
		if(safe && ((t.getEnd() - t.getStart()) > 0.5)) ts.push_back(t);
	}
	int size = int(ts.size());
	if(size>MAX) {
		double x = (double) (MAX);
		double y = (double) (size);
		double z = y/x;
		int u = 0;
		x = 0;
		while(u<size) {
			vc.extractGif(filename, path, uid, ts[u].getStart(), ts[u].getEnd());
			x += z;
			u = int(x);
		}
	} else {
		for(int i = 0; i<size; i++) {
			vc.extractGif(filename, path, uid, ts[i].getStart(), ts[i].getEnd());
		}
	}
	if(!vc.reset()) throw "Error clearing frames.";
}
void Filter::extractVids(const std::string& filename, const std::string& path, int uid, std::vector<Timestamp>& is) {
	std::vector<Timestamp> ts;
	for(auto t : is) {
		bool safe = true;
		for(auto test : ts) {
			if(overlapsTooMuch(t, test)) {
				safe = false;
				break;
			}
		}
		if(safe && ((t.getEnd() - t.getStart()) > 1.5)) ts.push_back(t);
	}
	int size = int(ts.size());
	if(size>MAX) {
		double x = (double) (MAX);
		double y = (double) (size);
		double z = y/x;
		int u = 0;
		x = 0;
		while(u<size) {
			vc.extractVid(filename, path, uid, ts[u].getStart(), ts[u].getEnd());
			x += z;
			u = int(x);
		}
	} else {
		for(int i = 0; i<size; i++) {
			vc.extractVid(filename, path, uid, ts[i].getStart(), ts[i].getEnd());
		}
	}
	if(!vc.reset()) throw "Error clearing frames.";
}
