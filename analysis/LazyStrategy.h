#pragma once
#include <vector>
#include <string>

class LazyStrategy : Strategy
{
public:
	std::vector<double> processVideo(const std::string& filename, int secondsPerClip);
};

