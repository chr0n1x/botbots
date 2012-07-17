#ifndef __MUTEX
#define __MUTEX

#include <pthread.h>

namespace mutex {

class Mutex
{

    // PRIVATE DATA
    pthread_mutex_t d_lock;

  public:
    // CREATORS

    Mutex();

    ~Mutex();

    // MANIPULATORS

    int tryLock();

    int lock();

    int unlock();

    pthread_mutex_t & getRawMutex();
    // TODO -- functions to handle mutex errors
};

class MutexGuard
{
    Mutex *d_mutex;

  public:
    MutexGuard(Mutex *mutex);
    ~MutexGuard();
};

//============================================================================
//                             INLINE DEFINITIONS
//============================================================================

                             //=================
                             // class MutexGuard
                             //=================
inline
MutexGuard::MutexGuard(Mutex *mutex)
: d_mutex(mutex)
{
    d_mutex->lock();
}

inline
MutexGuard::~MutexGuard()
{
    d_mutex->unlock();
}

                             //============
                             // class Mutex
                             //============
inline
Mutex::Mutex()
{
    pthread_mutex_init(&d_lock, NULL);
}

inline
Mutex::~Mutex()
{
    pthread_mutex_destroy(&d_lock);
}

inline
int Mutex::tryLock()
{
    return pthread_mutex_trylock(&d_lock);
}

inline
int Mutex::lock()
{
    return pthread_mutex_lock(&d_lock);
}

inline
int Mutex::unlock()
{
    return pthread_mutex_unlock(&d_lock);
}

}  // close namespace mutex

#endif
