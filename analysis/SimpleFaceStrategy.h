#include "Strategy.h"
class SimpleFaceStrategy : Strategy
{
public:
	std::vector<double> processVideo(const std::string & filename, int secondsPerClip);
};

