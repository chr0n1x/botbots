#include "condition_variable.h"

namespace condition_vars {

  pthread_cond_t & ConditionVariable::getRawCV() {
    return cv;
  }

  // TODO -- functions to handle CV errors
}
