#ifndef __SYNCHRONIZE
#define __SYNCHRONIZE

#include "mutex.h"
#include "condition_variable.h"

namespace synchronize {

  using namespace mutex;
  using namespace condition_vars;

  // TODO -- error handling

  inline waitOnCondition(ConditionVariable cv, Mutex mutex) {
    pthread_cond_wait( cv.getRawCV(), mutex.getRawMutex() );
  }

  inline raiseSignal(ConditionVariable cv) {
    pthread_cond_signal( cv.getRawCV() );
  }

  inline signalAll( ConditionVariable cv ) {
    pthread_cond_broadcast( cv.getRawCV() );
  }

}

#endif
