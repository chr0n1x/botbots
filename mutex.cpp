#include "mutex.h"

namespace mutex {

  pthread_mutex_t & Mutex::getRawMutex() {
    return d_lock;
  }

  // TODO -- functions to handle mutex errors

}
