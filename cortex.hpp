#ifndef __CORTEX
#define __CORTEX

#include <queue>

#include "internals.hpp"

namespace the_cortex {

  void* _cortex_thread(void*);
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

      void operator=(cortex_object& in) {
        worker_function = in.worker_function;
        object = in.object;
      }

      void execute() {
        if(worker_function && object)
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

        /*
        void pop() {
          pthread_mutex_lock(&queue_lock);

          if(que.size() > 0) {
            que.pop();
            pthread_mutex_unlock(&queue_lock);
          }
        }

        cortex_object& front() {
          return que.front();
        }
        */

        cortex_object front_pop() {
          pthread_mutex_lock(&queue_lock);

          cortex_object ret;
          // make sure that there are objects in the queue
          if(que.size() > 0) {
            // get the front as a copy, pop it off
            ret = que.front();
            que.pop();
          }
          // nothing in the queue; unlock the mutex, yield CPU
          // time to another thread (maybe the main to queue up objects)
          else {
            pthread_mutex_unlock(&queue_lock);
            //pthread_yield();
            sched_yield();
          }

          pthread_mutex_unlock(&queue_lock);
          return ret;
        }

        int size() {
          return que.size();
        }
    };

    /**
     *  FIELDS
     */
    ts_queue gate;
    pthread_t workers[MAX_CORTEX_THREADS];
    pthread_mutex_t gate_process_lock;
    bool workers_running;

    /**
     *  queue_object
     *  Adds a specialized cortex object to the queue
     *  and starts processing if possible
     */
    template <typename _obj>
    void queue_object(specialized_cortex_object<_obj> co) {
      gate.push(co);
    }

    public:

      cortex() {
        pthread_mutex_init( &gate_process_lock, NULL );
        set_process_flag(true);
      }

      ~cortex() {
        set_process_flag(false);
        pthread_mutex_destroy( &gate_process_lock );
      }

      /**
       *  queue_task
       *  Given a function pointer and the argument that it requires,
       *  create a cortex_object and push it into the queue
       */
      template <typename _obj>
      void queue_task( _obj* object, void* (*thread_func)(void*) ) {
        specialized_cortex_object<_obj> sco(object, thread_func);
        //queue_object(sco);
        gate.push(sco);
      }

      /**
       *  process_next_gate_element()
       */
      void process_next_gate_element() {
        //pthread_mutex_lock( &gate_process_lock );
        gate.front_pop().execute();
        //pthread_mutex_unlock( &gate_process_lock );
      }

      /**
       *  process_cortex_iteratively
       *  Goes through the entire queue, processing all tasks 1 by 1
       */
      void process_gate_iteratively() {
        workers_running = 0;
        while(gate.size() > 0) {
          process_next_gate_element();
        }
      }

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
       *  status
       *  whether the cortex has signaled its threads to process or not
       */
      bool status() {
        return workers_running;
      }

      /**
       *  tasks_queued_in_gate
       *  returns the number of tasks in the queue
       */
      int tasks_queued_in_gate() {
        return gate.size();
      }
  };

  /**
   *  _cortex_thread
   *  Thread function that the workers use to read from the queue
   */
  void* _cortex_thread(void* arg) {
    cortex* core = (cortex*) arg;
    while(core->status()) {
      if(core->tasks_queued_in_gate()) {
        core->process_next_gate_element();
      }
    }
    return NULL;
  }
}

#endif
