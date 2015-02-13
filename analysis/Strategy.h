class Strategy
{
public:
	//Abstract method (class) for strategies.
	virtual std::vector<double> processVideo(const std::string & filename, int secondsPerClip) = 0;
};
