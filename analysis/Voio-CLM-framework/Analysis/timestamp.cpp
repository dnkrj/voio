#include "timestamp.h"

//Timestamp member functions.
Timestamp::Timestamp(double a, double b) : start(a), end(b) {}
Timestamp::Timestamp(void) {}
void Timestamp::create(double a, double b) {
	start = a;
	end = b;
}
Timestamp::~Timestamp(void) {}
double Timestamp::getStart(void) {return start;}
double Timestamp::getEnd(void) {return end;}
