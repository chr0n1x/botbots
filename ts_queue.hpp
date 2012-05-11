#ifndef __TS_QUEUE
#define __TS_QUEUE

#include <deque>
#include "internals.hpp"

/**
 *  TEMPLATE: _obj
 *  CLASS:    ts_queue
 *
 *  class wrapper to make the container thread safe
 */
template <typename _obj>
class ts_queue {

  deque<_obj> que;
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
     *  (_obj&) x the object to be queued
     *
     *  Push an object to the beginning of the queue
     */
    void push(const _obj &x) {
      while(pthread_mutex_trylock(&queue_lock) != 0) sched_yield();
      que.push_front(x);
      pthread_mutex_unlock(&queue_lock);
    }

    /**
     *  back_pop()
     *
     *  Thread-safe method to retrieve the last (oldest) _obj in the
     *  list and pop it off
     */
    _obj back_pop() {
      while(pthread_mutex_trylock(&queue_lock) != 0) sched_yield();

      _obj ret;
      // make sure that there are objects in the queue
      if(que.size() > 0) {
        // get the front as a copy, pop it off
        ret = que.back();
        que.pop_back();
      }
      pthread_mutex_unlock(&queue_lock);

      return ret;
    }

    size_t size() {
      return que.size();
    }
};

#endif
