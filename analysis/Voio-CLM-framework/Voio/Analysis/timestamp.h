#ifndef _TIMESTAMP
#define _TIMESTAMP
/*
Timestamp class with integer variables defining start and end of video segment in milliseconds.
*/
class Timestamp {
	double start;
	double end;
	public:
		Timestamp();
		Timestamp(double a, double b);
		void create(double a, double b);
		~Timestamp();
		double getStart(void);
		double getEnd(void);
};
#endif
