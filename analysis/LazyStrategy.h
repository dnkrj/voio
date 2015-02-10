#pragma once
#include <vector>
#include <string>

class LazyStrategy
{
public:
	std::vector<double> processVideo(const std::string& filename, int secondsPerClip);
};

