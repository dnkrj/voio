#include "Strategy.h"

class SimpleFaceStrategy : Strategy
{
public:
	std::vector<Timestamp> processVideo(const std::string& filename, int secondsPerClip);
};

