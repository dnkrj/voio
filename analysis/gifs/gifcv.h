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

/*
Class representing a produced GIF with file path, user ID and GIF ID and whether or not
it has been published.
*/

class GIF {
	int userID;
	int gifID;
	std::string gifPath;
	std::string videoPath;
	bool published;
	public:
		int getUID(void);
		int getGID(void);
		bool isPublished(void);
		std::string getVideoPath(void);
		std::string getPath(void);
		std::string publish(void);// Returns filepath of GIF
		void create(int uid, int gid, std::string vP);
		GIF();
		~GIF();
};

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
	public:
  		VideoConverter(int sx, int sy);
  		~VideoConverter();
  		bool save(const char* filename);
  		bool addFrame(uint8_t* data,  float dt);
		bool clear(void);
		bool reset(void);
		GIF extractGif(const std::string& src, int uid, double start, double end);
		GIF& extractVid(const std::string& src, int uid, long start, long end);
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
		std::vector<GIF> extractGifs(const std::string& filename, int uid, std::vector<Timestamp>& ts);
};
#endif
