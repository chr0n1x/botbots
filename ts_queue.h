#ifndef __TS_QUEUE
#define __TS_QUEUE

#include <deque>
#include "mutex.h"
#include "internals.h"

/**
 *  TEMPLATE: ELEMTYPE
 *  CLASS:    ts_queue
 *
 *  class wrapper to make the container thread safe
 */
template <typename ELEMTYPE>
class ts_queue {

  deque<ELEMTYPE> d_queue;
  mutex::Mutex           d_lock;

  public:

    /**
     *  push()
     *  (ELEMTYPE&) x the object to be queued
     *
     *  Push an object to the beginning of the queue
     */
    void push(const ELEMTYPE &x)
    {
        d_lock.lock();
        d_queue.push_front(x);
        d_lock.unlock();
    }

    /**
     *  back_pop()
     *
     *  Thread-safe method to retrieve the last (oldest) ELEMTYPE in the
     *  list and pop it off
     */
    ELEMTYPE back_pop()
    {
        d_lock.lock();

        ELEMTYPE ret;

        // make sure that there are objects in the queue

        // TODO - the check for empty should be done while the lock is held,
        // since the caller thread will be none the wiser that it has gotten a
        // default constructed object.

        if(d_queue.size() > 0) {
            // get the front as a copy, pop it off
            ret = d_queue.back();
            d_queue.pop_back();
        }
        d_lock.unlock();

        return ret;
    }

    size_t size()
    {
        // TODO - not thread safe
        return d_queue.size();
    }
};

#endif
