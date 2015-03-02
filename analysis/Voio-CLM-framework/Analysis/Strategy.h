#include <vector>
#include <iostream>
#include "timestamp.h"
class Strategy
{
public:
	//Abstract method (class) for strategies.
	virtual std::vector<Timestamp> processVideo(const std::string & filename, int secondsPerClip) = 0;
};
