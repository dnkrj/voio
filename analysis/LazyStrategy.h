#pragma once
#include <vector>

class LazyStrategy
{
public:
	std::vector<double> processVideo(const std::string & filename, int secondsPerClip);
};

