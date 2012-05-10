#ifndef __CORTEX
#define __CORTEX

#include <queue>

#include "internals.hpp"

namespace the_cortex {

  void* _cortex_thread(void*);
  static const int MAX_CORTEX_THREADS = 8;

  /**
   *  CLASS: cortex
   *
   *  Implementation of a thread pool using a thread
   *  safe queue
   */
  class cortex {

    /**
     *  STRUCT: cortex_object
     *
     *  Most basic unit of the cortex
     */
    struct cortex_object {
      void* (*worker_function)(void*);
      void* object;

      cortex_object() {
        object = NULL;
        worker_function = NULL;
      }

      void operator=(cortex_object& in) {
        worker_function = in.worker_function;
        object = in.object;
      }

      bool functional() {
        return (object != NULL && worker_function != NULL);
      }

      void execute() {
        (*worker_function)(object);
      }
    };

    /**
     *  STRUCT: specialized_cortex_object
     *
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
     *
     *  class wrapper to make the container thread safe
     *  Might make this overwrite queue<cortex_object> instead
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

        /**
         *  push()
         *  (cortex_object&) x the object to be queued
         *
         *  Push an object to the end of the queue
         */
        void push(const cortex_object &x) {
          while(pthread_mutex_trylock(&queue_lock) != 0) sched_yield();
          que.push(x);
          pthread_mutex_unlock(&queue_lock);
        }

        /**
         *  front_pop()
         *
         *  Thread-safe method to retrieve the front cortex_object in the
         *  queue and pop it off
         */
        cortex_object front_pop() {
          while(pthread_mutex_trylock(&queue_lock) != 0) sched_yield();

          cortex_object ret;

          // make sure that there are objects in the queue
          if(que.size() > 0 && que.front().functional()) {
            // get the front as a copy, pop it off
            ret = que.front();
            que.pop();
          }
          pthread_mutex_unlock(&queue_lock);

          return ret;
        }

        size_t size() {
          return que.size();
        }
    };

    /**
     *  FIELDS
     */
    ts_queue gate;
    pthread_t workers[MAX_CORTEX_THREADS];
    bool workers_running;
 
    public:

      cortex() {
        set_process_flag(true);
      }

      ~cortex() {
        set_process_flag(false);
      }

      /**
       *  queue_task()
       *  (void*) (*thread_func)(void*) pointer to the function that knows how to handle
       *  (_obj*) object, the data to be processed
       *
       *  Given a function pointer and the argument that it requires,
       *  create a cortex_object and push it into the queue
       */
      template <typename _obj>
      void queue_task( _obj* object, void* (*thread_func)(void*) ) {
        specialized_cortex_object<_obj> sco(object, thread_func);
        gate.push(sco);
      }

      /**
       *  process_next_gate_element()
       */
      void process_next_gate_element() {
        cortex_object co = gate.front_pop();
        if(co.functional())
          co.execute();
      }

      /**
       *  process_cortex_iteratively()
       *
       *  Goes through the entire queue, processing all tasks 1 by 1
       */
      void process_gate_iteratively() {
        workers_running = false;
        while(gate.size() > 0) {
          process_next_gate_element();
        }
      }

      /**
       *  set_process_flag()
       *  (bool) set The value to set workers_running to
       *
       *  Sets the workers_running flag
       *  Based on the new state of the flag, the threads are 
       *  Either re-created or joined with the main thread.
       */
      void set_process_flag(bool set) {
        if(set == workers_running)
          return;

        workers_running = set;

        // workers ARE dead -- re-init
        if(workers_running) {
          void * arg = (void*) this;
          for(int i=0; i < MAX_CORTEX_THREADS; ++i) {
            pthread_create( &workers[i], NULL, _cortex_thread, arg );
          }
        }
        // wait for workers to join 
        else {
          for(int i=0; i < MAX_CORTEX_THREADS; ++i) {
            pthread_join( workers[i], NULL );
          }
        }
      }

      /**
       *  wait_for_empty_queue()
       *  (bool) terminate Make the threads join
       *
       *  TODO: find a way to put the calling thread to sleep and then wake
       */
      void wait_for_empty_queue(bool terminate) {
        while(gate.size())//tasks_queued_in_gate())
          sched_yield();

        if(terminate)
          set_process_flag(false);
      }

      /**
       *  process_signal()
       *
       *  whether the cortex has signaled its threads to process or not
       */
      bool process_signal() {
        return workers_running;
      }

      /**
       *  objects_in_gate()
       *
       *  Return the number of objects queued in the gate
       */
      size_t objects_in_gate() {
        return gate.size();
      }
  };

  /**
   *  _cortex_thread()
   *  (void*) arg The cortex to be read from
   *
   *  Thread function that the workers use to access the cortex that they
   *  belong to and process the next cortex_object
   */
  void* _cortex_thread(void* arg) {
    cortex* core = (cortex*) arg;
    while(core->process_signal()) {
      core->process_next_gate_element();
    }
    return NULL;
  }
}

#endif
