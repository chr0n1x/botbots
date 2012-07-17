#include <vector>
#include "synchronize.h"

std::vector <pthread_t>           _threads;

condition_vars::ConditionVariable _thread_print_cv;
condition_vars::ConditionVariable _thread_die_cv;

mutex::Mutex                      _thread_print_lock;
mutex::Mutex                      _print_lock;
mutex::Mutex                      _die_lock;

#include <iostream>

void * worker_thread(void *arg) {

  int t_num = (int) arg;

  OP_ON_LOCK(
    _print_lock,
    std::cout << "\tWorkerThread" << t_num << " initialized" << std::endl;
  )

  OP_ON_CV_LOCK(
    _thread_print_cv,
    _thread_print_lock,
    OP_ON_LOCK(
      _print_lock,
      std::cout << "\tworker_thread" << t_num << "::print" << std::endl;
    )
  )

  OP_ON_CV_LOCK(
    _thread_die_cv,
    _die_lock,
    OP_ON_LOCK(
      _print_lock,
      std::cout << "\tworker_thread" << t_num << "::die" << std::endl;
    )
  )

  pthread_exit(NULL);
}

#define SLEEP true

#define WRAP_SLEEP( _THE_CODES )  \
  if(SLEEP)                       \
    sleep(1);                     \
  _THE_CODES                      \
  if(SLEEP)                       \
    sleep(1);

#define _THREADS  2

int main (int argc, char ** argv) {

  _threads.resize(_THREADS);
  int zero=0, one=1;

  std::cout << "STARTING" << std::endl;

  WRAP_SLEEP(
    OP_ON_LOCK(
      _print_lock,
      std::cout << "Threads created" << std::endl;
      pthread_create(&_threads[0], NULL, worker_thread, (void*) zero);
      pthread_create(&_threads[1], NULL, worker_thread, (void*) one);
    )
  )
  OP_ON_LOCK( _print_lock, std::cout << std::endl; )

  WRAP_SLEEP(
    OP_ON_LOCK(
      _print_lock,
      std::cout << "Raising print signal..." << std::endl;
      synchronize::signalAll( _thread_print_cv );
    )
  )
  OP_ON_LOCK( _print_lock, std::cout << std::endl; )

  WRAP_SLEEP(
    OP_ON_LOCK(
      _print_lock,
      std::cout << "Raising die signal..." << std::endl;
      synchronize::signalAll( _thread_die_cv );
    )
  )
  OP_ON_LOCK( _print_lock, std::cout << std::endl; )

  OP_ON_LOCK( _print_lock, std::cout << "Waiting for threads to join..." << std::endl; )
  for(int i=0; i<_threads.size(); ++i) {
    pthread_join(_threads[i], NULL);
  }
  OP_ON_LOCK( _print_lock, std::cout << "main() end" << std::endl; )

  return 0;
}
