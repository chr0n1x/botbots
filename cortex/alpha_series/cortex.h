#ifndef __CORTEX
#define __CORTEX

#include "task.h"
#include "synchronize.h"

#include <queue>
#include <vector>

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

namespace the_cortex {

/**
 *  CLASS: cortex
 *
 *  Implementation of a thread pool using a thread
 *  safe queue
 */
class Cortex
{

    static const int MAX_THREADS = 8;

    // PRIVATE DATA

    condition_vars::ConditionVariable d_cv_has_work;
    mutex::Mutex                      d_mutex;
    std::queue<functional::Task*>     d_gate;
    std::vector<pthread_t>            d_workers;
    bool                              d_processing;

  public:

    Cortex(int numThreads = MAX_THREADS);

    ~Cortex();

    /**
     *  queue_task()
     *  Task* pointer to the task to execute. Note that the cortex will own
     *  this pointer after being passed and should not be modified after
     *  passing.
     *
     *  Enqueue this function onto the threadpool
     */
    void enqueue_task(functional::Task *task);

    /**
     *  process_next_gate_element()
     */
    bool process_next_function();

    /**
     *  process_gate_iteratively()
     *
     *  Goes through the entire queue, processing all tasks 1 by 1 in the
     *  calling thread
     */
    void process_gate_iteratively();

    /**
     *  start()
     *
     * Spawn the configured number of threads and begin processing the queue
     */
    void start();

    /**
     *  stop()
     *
     * Join active threads and end processing
     */
    void stop();


    /**
     *  wait_for_empty_queue()
     *
     * Block until the queue is empty
     */
    void drain();

    /**
     *  process_signal()
     *
     *  Return true if the cortex is processing and false otherwise
     */
    bool isStarted();

   /**
    *  objects_in_gate()
    *
    *  Return the number of objects queued in the gate
    */
    size_t size();
};

}

#endif
