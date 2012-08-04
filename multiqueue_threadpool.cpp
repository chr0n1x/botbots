#include "multiqueue_threadpool.h"

#include <iostream>
#include <climits>

namespace {

void *worker_thread(void *queuePackage)
{
    QueueAndLock *dataPackage = (QueueAndLock*)queuePackage;

    volatile bool& processing            = dataPackage->d_running;
    mutex::Mutex&  qlock                 = dataPackage->d_qlock;
    std::queue<functional::Task*>& queue = dataPackage->d_queue;

    while(processing)
    {
        if (0 != dataPackage->d_task_sem.tryWait()) {
            dataPackage->d_empty_sem.post();
            dataPackage->d_task_sem.wait();
            if (!processing) return NULL;
            dataPackage->d_empty_sem.tryWait();
        }

        if (!processing) return NULL;
        qlock.lock();
        functional::Task *job = queue.front();
        queue.pop();
        qlock.unlock();
        job->execute();
        delete job;
    }

    return NULL;
}

}

MultiQueueThreadpool::MultiQueueThreadpool(int numThreads)
: d_processing(false)
{
    if (numThreads > MAX_THREADS) {
        numThreads = MAX_THREADS;
    }
    d_workers.resize(numThreads);
    d_queues.resize(numThreads);
}

MultiQueueThreadpool::~MultiQueueThreadpool()
{
    stop();

    for (int i = 0; i < d_queues.size(); ++i)
    {
        QueueAndLock& currQ = d_queues[i];
        currQ.d_qlock.lock();
        while(!currQ.d_queue.empty())
        {
            functional::Task *job = currQ.d_queue.front();
            currQ.d_queue.pop();
            delete job;
            currQ.d_task_sem.wait(); // sanity check
        }
        currQ.d_qlock.unlock();
    }
}

void MultiQueueThreadpool::start()
{
    if(d_processing) {
        return;
    }

    d_processing = true;

    for(int i = 0; i < d_workers.size(); ++i)
    {
        d_queues[i].d_running = true;
        pthread_create(&d_workers[i],
                       NULL,
                       worker_thread,
                       (void*)&d_queues[i]);
    }
}

void MultiQueueThreadpool::stop()
{
    if (!d_processing) {
        return;
    }
    d_processing = false;

    for(int i=0; i < d_queues.size(); ++i)
    {
        d_queues[i].d_running = false;
        d_queues[i].d_task_sem.post();
    }

    for(int i=0; i < d_workers.size(); ++i)
    {
        pthread_join(d_workers[i], NULL);
    }
}

void MultiQueueThreadpool::enqueue_task(functional::Task *task)
{
    // add to lowest pending jobs queue
    int lowestIdx = 0;
    int lowestAmt = INT_MAX;
    for (int i = 0; i < d_queues.size(); ++i)
    {
        QueueAndLock& currQ = d_queues[i];
        currQ.d_qlock.lock();
        int size = currQ.d_queue.size();
        currQ.d_qlock.unlock();
        if (size < lowestAmt) {
            lowestAmt = size;
            lowestIdx = i;
        }

    }

    QueueAndLock& lowQueue = d_queues[lowestIdx];
    lowQueue.d_qlock.lock();
    lowQueue.d_queue.push(task);
    lowQueue.d_task_sem.post();
    lowQueue.d_qlock.unlock();
}

void MultiQueueThreadpool::drain()
{
    if (!d_processing) return;
    for (int i = 0; i < d_queues.size(); ++i)
    {
        while (true)
        {
            d_queues[i].d_empty_sem.wait();

            // There is a chance that the worker could have run out, upped the
            // empty sem, gone to sleep, had work added, drain gets called,
            // this thread downs the sem, even though there is still work.
            d_queues[i].d_qlock.lock();
            bool isEmpty = d_queues[i].d_queue.empty();
            d_queues[i].d_qlock.unlock();
            if (isEmpty) break;
        }
    }
}

bool MultiQueueThreadpool::isStarted() const
{
    return d_processing;
}
