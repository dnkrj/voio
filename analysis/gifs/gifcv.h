/*
Class representing a produced GIF with file path, user ID and GIF ID and whether or not
it has been published.
*/

class GIF {
	int userID;
	int gifID;
	string gifPath;
	string videoPath;
	bool published;
	public:
		int getUID(void);
		int getGID(void);
		bool isPublished(void);
		string getVideoPath(void);
		string getPath(void);
		string publish(void);// Returns filepath of GIF
		void create(int uid, int gid, string vP);
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
	VideoCapture cap;
	vector<frame_t> frames;
	vector<int> delay;
	bool addLoop(GifFileType *gf);
	public:
  		VideoConverter(int sx, int sy);
  		~VideoConverter();
  		bool save(const char* filename);
  		bool addFrame(uint8_t* data,  float dt);
		bool clear(void);
		bool reset(void);
		GIF extractGif(const string& src, int uid, long start, long end);
		GIF& extractVid(const string& src, int uid, long start, long end);
};

/*
Timestamp class with integer variables defining start and end of video segment in milliseconds.
*/
class Timestamp {
	long start;
	long end;
	public:
		Timestamp();
		Timestamp(long a, long b);
		void create(long a, long b);
		~Timestamp();
		long getStart(void);
		long getEnd(void);
};

/*
Filter class which should be called by analysis algorithms and will output upto 15 GIFs from given timestamps.
Currently only picks evenly distributed segments across video, but can implement ordering based on how interesting video is on a non binary scale.
*/
class Filter {
	int MAX = 15;
	VideoConverter vc;
	public:
		Filter();
		~Filter();
		vector<GIF> extractGifs(const string& filename, int uid, vector<Timestamp>& ts);
};
