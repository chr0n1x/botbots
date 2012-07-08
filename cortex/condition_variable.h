#ifndef __CONDITION_VARIABLE
#define __CONDITION_VARIABLE

#include <pthread.h>

namespace condition_vars {

  class ConditionVariable
  {
    pthread_cond_t cv;
      
    public:
        ConditionVariable();
        ~ConditionVariable();

        pthread_cond_t * getRawCV();
  };

  inline ConditionVariable::ConditionVariable() {
    pthread_cond_init( &cv, NULL );
  }

  inline ConditionVariable::~ConditionVariable() {
    pthread_cond_destroy( &cv );
  }

}

#endif
