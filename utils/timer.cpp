#include "timer.h"

timer::timer()
{
	clock_gettime(CLOCK_REALTIME, &beg_); 
}

double timer::elapsed()
{
	clock_gettime(CLOCK_REALTIME, &end_);
	return end_.tv_sec - beg_.tv_sec + (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
}
    
void timer::reset()
{
	clock_gettime(CLOCK_REALTIME, &beg_);
}