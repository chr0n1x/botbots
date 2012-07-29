#ifndef INCLUDED_MULTIQUEUETHREADPOOL_H
#define INCLUDED_MULTIQUEUETHREADPOOL_H

#include "mutex.h"
#include "synch_semaphore.h"
#include "task.h"

#include <vector>
#include <queue>

#include <pthread.h>

struct QueueAndLock {
    synch::Semaphore              d_task_sem;
    synch::Semaphore              d_empty_sem;
    mutex::Mutex                  d_qlock;
    std::queue<functional::Task*> d_queue;
    volatile bool                 d_running;

    QueueAndLock() { d_running = false; }
};

class MultiQueueThreadpool
{
    static const int MAX_THREADS = 8;

    std::vector< QueueAndLock > d_queues;
    std::vector<pthread_t>      d_workers;
    bool                        d_processing;

  public:

    MultiQueueThreadpool(int numThreads = MAX_THREADS);

    ~MultiQueueThreadpool();

    void start();
    // Start the threadpool. If this threadpool has already been started, a
    // call to this function is a no-op. The behavior is undefined unless
    // stop() has not yet been called.

    void stop();
    // Stop the threadpool. If this threadpool is stopped then a call to this
    // function is a no-op.

    void enqueue_task(functional::Task *task);
    // Add this function to the queue.  The threadpool will own this pointer
    // after it is passed and will delete it when it has been processed.

    void drain();
    // Block the calling function until the queue is drained. Behavior is
    // undefined if calls to enqueue_task are made during a call to drain.

    bool isStarted() const;
    // Return true if the threadpool is started and false otherwise.

    // size_t numPendingJobs() const;
    // Return the number of jobs to be processed.
};

#endif
