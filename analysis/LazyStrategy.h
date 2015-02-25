
#include <vector>
#include <string>
#include "gifs/gifcv.h"
#include "Strategy.h"

#ifndef LAZYSTRAT
#define LAZYSTRAT

class LazyStrategy : Strategy
{
public:
	std::vector<Timestamp> processVideo(const std::string& filename, int secondsPerClip);
};

#endif
