#ifndef __CORTEX
#define __CORTEX

#include <queue>

#include "internals.hpp"

namespace the_cortex {

  static const int MAX_CORTEX_THREADS = 4;

  /**
   *  CLASS: cortex
   *  Implementation of a thread pool using a thread
   *  safe queue
   */
  class cortex {

    /**
     *  STRUCT: cortex_object
     *  Most basic unit of the cortex
     */
    struct cortex_object {
      void* (*worker_function)(void*);
      void* object;

      void execute() {
        (*worker_function)(object);
      }
    };

    /**
     *  STRUCT: specialized_cortex_object
     *  "Atomic" units that are queued for the worker threads
     *  to process
     */
    template <typename _obj>
    struct specialized_cortex_object : public cortex_object {
      specialized_cortex_object(_obj* iobj, void* (*ifunc)(void*)) {
        worker_function = ifunc;
        object = (void*) iobj;
      }
    };

    /**
     *  CLASS: ts_queue
     *  class wrapper to make the container thread safe
     */
    class ts_queue {

      queue<cortex_object> que;
      pthread_mutex_t queue_lock;

      public:
        ts_queue() {
          pthread_mutex_init(&queue_lock, NULL);
        }

        ~ts_queue() {
          pthread_mutex_destroy(&queue_lock);
        }

        void push(const cortex_object &x) {
          pthread_mutex_lock(&queue_lock);
          que.push(x);
          pthread_mutex_unlock(&queue_lock);
        }

        void pop() {
          pthread_mutex_lock(&queue_lock);
          que.pop();
          pthread_mutex_unlock(&queue_lock);
        }

        cortex_object & front() {
          return que.front();
        }

        int size() {
          return que.size();
        }
    };

    /**
     *  FIELDS
     */
    ts_queue gate;

    /**
     *  queue_object
     *  Adds a specialized cortex object to the queue
     *  and starts processing if possible
     */
    template <typename _obj>
    void queue_object(specialized_cortex_object<_obj> co) {
      gate.push(co);
 
      // logic to start task processing
    }

    public:

      /**
       *  queue_task
       *  Given a function pointer and the argument that it requires,
       *  create a cortex_object and push it into the queue
       */
      template <typename _obj>
      void queue_task( _obj* object, void* (*thread_func)(void*) ) {
        specialized_cortex_object<_obj> sco(object, thread_func);
        queue_object(sco);
      }

      /**
       *  process_queue_iteratively
       *  Goes through the entire queue, processing all tasks 1 by 1
       */
      void process_queue_iteratively() {
        while(gate.size() > 0) {
          gate.front().execute();
          gate.pop();
        }
      }
  };

}

#endif
