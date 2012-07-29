#include "multiqueue_threadpool.h"
#include "boundjob.h"
#include "mutex.h"
#include "task.h"

#include <cassert>
#include <iostream>

using namespace mutex;
using namespace functional;
using namespace std;

void increment(int *num, Mutex *lock)
{
    lock->lock();
    ++(*num);
    lock->unlock();
}

int main(int argc, char** argv)
{
    const int NUMTHREADS = 8;
    const int NUMJOBS    = 100000;
    int jobTracker       = 0;
    Mutex lock;
    MultiQueueThreadpool mqt(NUMTHREADS);

    mqt.start();

    for(int i = 0; i < NUMJOBS; ++i)
    {
        mqt.enqueue_task(BindUtil::bind(&increment, &jobTracker, &lock));
    }

    mqt.drain();

    cout << jobTracker << endl;

    assert(NUMJOBS == jobTracker);

    mqt.stop();
}
