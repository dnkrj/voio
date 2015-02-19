#include "Strategy.h"
class SimpleFaceStrategy : Strategy
{
public:
	std::vector<TimeStamp> processVideo(const std::string& filename, int secondsPerClip) override;
};

