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
#include "opencv2/video/video.hpp"
#endif

typedef std::vector<GifByteType> frame_t;
std::vector<std::string> split(const std::string& s, char delim);

/*Based on Marco Tarini's AnimatedGifSaver at
http://sourceforge.net/p/giflib/patches/6/
Updated to use giflib V5 and integrate with OpenCV.
Constructor takes two integers - width and height of GIF frames.
Pass source video filename, user ID, start timestamp and end timestamp to extractGif which will save a gif from this interval.
Note: extractVid() doesn't currently work.
*/

class VideoConverter {
	int gid;
	int gifsx;
	int gifsy;
	int paletteSize;
	ColorMapObject* outputPalette;
	cv::VideoCapture cap;
	std::vector<frame_t> frames;
	std::vector<int> delay;
	bool addLoop(GifFileType *gf);
	bool addFrame(uint8_t* data,  float dt);
	public:
		std::string getPath(int uid, int gid, std::string vP, const std::string& prepath);
		std::string getVideoPath(int uid, int vid, std::string vP, const std::string& prepath);
		std::string getFinalPath(int uid, int vid, std::string vP, const std::string& prepath);
  		VideoConverter(int sx, int sy);
  		~VideoConverter();
  		bool save(const char* filename);
		bool clear(void);
		bool reset(void);
		void extractGif(const std::string& src, const std::string& path, int uid, double start, double end);
		void extractVid(const std::string& src, const std::string& path, int uid, double start, double end);
};

/*
Timestamp class with integer variables defining start and end of video segment in milliseconds.
*/
class Timestamp {
	double start;
	double end;
	public:
		Timestamp();
		Timestamp(double a, double b);
		void create(double a, double b);
		~Timestamp();
		double getStart(void);
		double getEnd(void);
};

/*
Filter class which should be called by analysis algorithms and will output upto 15 GIFs from given timestamps.
Currently only picks evenly distributed segments across video, but can implement ordering based on how interesting video is on a non binary scale.
*/
class Filter {
	const static int MAX = 15;
	VideoConverter vc;
	public:
		Filter();
		~Filter();
		void extractGifs(const std::string& filename, const std::string& path, int uid, std::vector<Timestamp>& ts);
		void extractVids(const std::string& filename, const std::string& path, int uid, std::vector<Timestamp>& ts);
};
#endif
