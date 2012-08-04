#ifndef INCLUDED_SEMAPHORE_H
#define INCLUDED_SEMAPHORE_H

#include <semaphore.h>

namespace synch {

class Semaphore
{
    // PRIVATE DATA
    sem_t d_semaphore;

  public:
    Semaphore();

    ~Semaphore();

    void post();

    void wait();

    int tryWait();

    int getValue();
};

//----------------------------------------------------------------------------
//                            INLINE DEFINITIONS
//----------------------------------------------------------------------------
inline
Semaphore::Semaphore()
{
    sem_init(&d_semaphore, 0, 0);
}

inline
Semaphore::~Semaphore()
{
    sem_destroy(&d_semaphore);
}

inline
void Semaphore::post()
{
    sem_post(&d_semaphore);
}

inline
void Semaphore::wait()
{
    sem_wait(&d_semaphore);
}

inline
int Semaphore::tryWait()
{
    return sem_trywait(&d_semaphore);
}

inline
int Semaphore::getValue()
{
    int val;
    sem_getvalue(&d_semaphore, &val);
    return val;
}

} // close namespace synch

#endif
