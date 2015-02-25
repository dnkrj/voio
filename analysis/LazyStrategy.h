#pragma once
#include <vector>
#include <string>
#include "gifs/gifcv.h"

class LazyStrategy : Strategy
{
public:
	std::vector<Timestamp> processVideo(const std::string& filename, int secondsPerClip);
};

