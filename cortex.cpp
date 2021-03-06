#include "cortex.h"

namespace the_cortex {

namespace {
 /**
  *  cortex_worker_thread()
  *  (void*) arg The cortex to be read from
  *
  *  Thread function that the workers use to access the cortex that they
  *  belong to and process the next cortex_object
  */
void* cortex_worker_thread(void* arg)
{
    Cortex* core = (Cortex*) arg;
    while(core->isStarted())
    {
        core->process_next_function();
    }
    return NULL;
}

} // close unnamed namespace

Cortex::Cortex(int numThreads)
: d_processing(false)
{
    if (numThreads > MAX_THREADS) {
        numThreads = MAX_THREADS;
    }
    d_workers.resize(numThreads);
}

Cortex::~Cortex()
{
    // delete any remaining tasks
    stop();

    d_mutex.lock();
    while (!d_gate.empty())
    {
        functional::Task *job = d_gate.front();
        d_gate.pop();
        delete job;
    }
    d_mutex.unlock();
}

void Cortex::enqueue_task(functional::Task *task)
{
  d_mutex.lock();
  d_gate.push(task);
  synchronize::signalAll(d_cv_has_work);
  d_mutex.unlock();
}

/**
 *  process_next_function()
 */
bool Cortex::process_next_function()
{
    functional::Task *ret = NULL;

    d_mutex.lock();
    while(d_processing)
    {
        if (d_gate.empty()) {
            synchronize::signalAll(d_cv_queue_empty);
            synchronize::waitOnCondition(d_cv_has_work, d_mutex);
        }
        else {
            break;
        }
    }
    if(d_processing) {
        ret = d_gate.front();
        d_gate.pop();
    }
    d_mutex.unlock();

    if(d_processing) {
        ret->execute();
        delete ret;
    }
    return true;
}

/**
 *  process_gate_iteratively()
 *
 *  Goes through the entire queue, processing all tasks 1 by 1 in the
 *  calling thread
 */
void Cortex::process_gate_iteratively()
{
    stop();

    d_mutex.lock();
    while(!d_gate.empty())
    {
        functional::Task *job = d_gate.front();
        d_gate.pop();
        job->execute();
        delete job;
    }
    d_mutex.unlock();
}

/**
 *  start()
 *
 * Spawn the configured number of threads and begin processing the queue
 */
void Cortex::start()
{
    if(!d_processing) {
        d_processing = true;

        for(int i = 0; i < d_workers.size(); ++i)
        {
            pthread_create(&d_workers[i], NULL,
                           cortex_worker_thread, (void*)this);
        }
    }

    return;
}

/**
 *  stop()
 *
 * Join active threads and end processing
 */
void Cortex::stop()
{
    if (d_processing) {
        d_processing = false;
        synchronize::signalAll(d_cv_has_work);

        for(int i=0; i < d_workers.size(); ++i)
        {
            pthread_join(d_workers[i], NULL);
        }
    }

    return;
}


/**
 *  wait_for_empty_queue()
 *
 * Block until the queue is empty
 */
void Cortex::drain()
{
    bool stillProcessing = true;
    while (stillProcessing)
    {
        d_mutex.lock();
        stillProcessing = !d_gate.empty();
        d_mutex.unlock();

        if (stillProcessing) {
            d_mutex.lock();
            synchronize::signalAll(d_cv_has_work);
            synchronize::waitOnCondition( d_cv_queue_empty, d_mutex );
            d_mutex.unlock();
        }
    }
}

/**
 *  process_signal()
 *
 *  Return true if the cortex is processing and false otherwise
 */
bool Cortex::isStarted()
{
    return d_processing;
}

/**
*  objects_in_gate()
*
*  Return the number of objects queued in the gate
*/
size_t Cortex::size()
{
    mutex::MutexGuard guard(&d_mutex);
    return d_gate.size();
}

}  // close the_cortex namespace
