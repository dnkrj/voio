#include <vector>
#include <iostream>
#include "gifs/gifcv.h"
#include "gifs/timestamp.h"
#include "gifs/fps.c"

#ifndef STRAT
#define STRAT
class Strategy
{
public:
	//Abstract method (class) for strategies.
	virtual std::vector<Timestamp> processVideo(const std::string & filename, int secondsPerClip) = 0;
};

#endif
