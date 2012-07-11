#ifndef __SYNCHRONIZE
#define __SYNCHRONIZE

#include "mutex.h"
#include "condition_variable.h"

using namespace mutex;
using namespace condition_vars;

namespace synchronize {

  // TODO -- error handling

  inline void waitOnCondition(ConditionVariable &cv, Mutex &mutex) {
    pthread_cond_wait( &cv.getRawCV(), &mutex.getRawMutex() );
  }

  inline void raiseSignal(ConditionVariable &cv) {
    pthread_cond_signal( &cv.getRawCV() );
  }

  inline void signalAll( ConditionVariable &cv ) {
    pthread_cond_broadcast( &cv.getRawCV() );
  }

  inline void lockMutex(Mutex &mutex) {
    mutex.lock();
  }

  inline void unlockMutex(Mutex &mutex) {
    mutex.unlock();
  }

}

#define OP_ON_LOCK( _MUTEX, _THE_CODES )        \
  synchronize::lockMutex( _MUTEX );             \
  _THE_CODES                                    \
  synchronize::unlockMutex( _MUTEX );
  

#define OP_ON_CV_LOCK( _CV, _MUTEX, _THE_CODES )\
  synchronize::lockMutex( _MUTEX );             \
  synchronize::waitOnCondition( _CV, _MUTEX );  \
  _THE_CODES                                    \
  synchronize::unlockMutex( _MUTEX );

#endif
