#ifndef __CORTEX
#define __CORTEX

#include <queue>
#include <map>

namespace the_cortex {

  /**
   *  CLASS: cortex
   *  Implementation of a thread pool using a thread
   *  safe queue
   */
  template <typename _obj>
  class cortex {

    /**
     *  CLASS: cortex_object
     *  "Atomic" units that are queued for the worker threads
     *  to process
     */
    class cortex_object {
      void* (worker_function)(void*);
      _obj * object;
    };

    /**
     *  CLASS: ts_queue
     *  class wrapper to make the container thread safe
     */
    class ts_queue : public queue<_obj> {
      queue<_obj> que;
      pthread_mutex_t queue_lock;

      public:
        ts_queue() {
          pthread_mutex_init(&queue_lock, NULL);
        }

        ~ts_queue() {
          pthread_mutex_destroy(&queue_lock);
        }

        void push(const _obj& x) {
          pthread_mutex_lock(&queue_lock);
          que.push(x);
          pthread_mutex_unlock(&queue_lock);
        }

        void pop() {
          pthread_mutex_lock(&queue_lock);
          que.pop();
          pthread_mutex_unlock(&queue_lock);
        }
    };

    public:

      /**
       *
       */
      void queue_task(_obj* object, void* (thread_func)(void*) ) {
        void * arg = (void*) object;
        thread_func(arg);
      }
  };

}

#endif
