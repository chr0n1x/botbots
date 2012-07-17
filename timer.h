#ifndef INCLUDED_TIMER_H
#define INCLUDED_TIMER_H

#include <time.h>
#include <sys/time.h>

/**
 *  Timer Class
 *
 *  DO NOT USE THIS CLASS AND ITS FUNCTIONS IN QUICK SUCCESSION
 *  (eg: Sean broke the Mutex class with this haha)
 */

namespace prof {

class Timer
{

    double d_start_time;
    double d_stop_time;

  public:
    Timer();

//    ~Timer(); // AUTO IMPLEMENT

    void start();

    double stop();

    double elapsed_time();
};

//----------------------------------------------------------------------------
//                            INLINE DEFINITIONS
//----------------------------------------------------------------------------

inline
Timer::Timer()
: d_start_time(0)
, d_stop_time(0)
{
}

/*
Timer::~Timer()
{
}
*/

inline
void Timer::start()
{
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    d_start_time = ((double)start_time.tv_sec * 1000000.0 +
                                 (double)start_time.tv_usec) / 1000000.0;
}

inline
double Timer::stop()
{
    struct timeval stop_time;
    gettimeofday(&stop_time, NULL);
    d_stop_time = ((double)stop_time.tv_sec * 1000000.0 +
                                 (double)stop_time.tv_usec) / 1000000.0;
}

inline
double Timer::elapsed_time()
{
    return d_stop_time - d_start_time;
}

} // close namespace prof

#endif
