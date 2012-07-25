#include "cortex.h"

namespace the_cortex {

namespace {
  /**
   *  cortex_worker_thread()
   *
   *  (void*) arg The cortex to be read from
   *
   *  Thread function that the workers use to access the cortex that they
   *  belong to and process the next cortex_object
   */
  void* cortex_worker_thread(void* arg)
  {
    Cortex* core = (Cortex*) arg;
    while(core->started())
    {
      core->process_next_function();
    }
    return NULL;
  }
}

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
  // wait for all of the threads to finish processing tasks and join
  stop();

  // delete any remaining tasks
  d_mutex.lock();
  while (!d_gate.empty())
  {
    functional::Task *job = d_gate.front();
    d_gate.pop();
    delete job;
  }
  d_mutex.unlock();
}

/**
 *  enqueue_task()
 *
 *  Task* pointer to the task to execute. Note that the cortex will own
 *  this pointer after being passed and should not be modified after
 *  passing.
 *
 *  Enqueue this function onto the threadpool
 */
void Cortex::enqueue_task(functional::Task *task)
{
  d_mutex.lock();
  d_gate.push(task);
  synchronize::signalAll(d_cv_has_work);
  d_mutex.unlock();
}

/**
 *  process_next_function()
 *
 *  Function used by the worker threads and ONLY by the worker threads
 */
bool Cortex::process_next_function()
{
  functional::Task *ret = NULL;

  d_mutex.lock();
  while(d_processing) {
    if (d_gate.empty()) {
      // if there are no more tasks when this thread accesses the queue
      // signal the parent thread that the queue is empty
      // then release d_mutex and wait for the has_work CV
      synchronize::raiseSignal(d_cv_queue_empty);
      synchronize::waitOnCondition(d_cv_has_work, d_mutex);
    }
    else {
      break;
    }
  }

  // make sure that the cortex is still flagged to process tasks
  // !d_processing implies that this thread was woken to join
  if(d_processing) {
    ret = d_gate.front();
    d_gate.pop();
  }
  d_mutex.unlock();

  // have another check for this so that it is possible to unlock the mutex asap
  if(d_processing)
    ret->execute();

  return true;
}

/**
 *  process_iteratively()
 *
 *  Goes through the entire queue, processing all tasks 1 by 1 in the
 *  calling thread
 */
void Cortex::process_iteratively()
{
  // case where thread-mode is running and this function is called
  stop();

  d_mutex.lock();
  while(!d_gate.empty())
  {
    d_gate.front()->execute();
    d_gate.pop();
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
      pthread_create(&d_workers[i], NULL, cortex_worker_thread, (void*)this);
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
 *  drain()
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
 *  started()
 *
 *  Return true if the cortex is processing and false otherwise
 */
bool Cortex::started()
{
  return d_processing;
}

/**
*  size()
*
*  Return the number of objects queued in the gate
*/
size_t Cortex::size()
{
  size_t ret = 0;
  d_mutex.lock();
  ret = d_gate.size();
  d_mutex.unlock();
}

}  // close the_cortex namespace
