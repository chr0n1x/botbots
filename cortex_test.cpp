#include <iostream>
#include "internals.hpp"
#include "cortex.hpp"

using namespace std;
using namespace the_cortex;

#define ELEMENTS 10000

/**
 *  Polymorphic classes
 */
class object {
  public:
    virtual void func1() { cout << "obj -- func1\n"; }
    virtual void func2() { cout << "obj -- func2"; }
};

class foo : public object {
  public:
    void func1() { cout << "foo -- func1\n"; }
    void func2() { cout << "foo -- func2\n"; }
};

class bar : public object {
  public:
    void func1() { cout << "bar -- func1\n"; }
    void func2() { cout << "bar -- func2\n"; }
};

/**
 *  Thread functions
 */
void *_threaded_a(void *arg) {
  object * obj = (object *) arg;
  obj->func1();
}

void *_threaded_b(void *arg) {
  object * obj = (object *) arg;
  obj->func2();
}

void fill(cortex &c, foo f, bar b) {
  for(int i=0; i<ELEMENTS; ++i) {
    c.queue_task(&f, &_threaded_a);
    c.queue_task(&f, &_threaded_b);
    c.queue_task(&b, &_threaded_a);
    c.queue_task(&b, &_threaded_b);
  }
}

int main(int argc, char ** argv) {

  if(argc != 2) {
    cout << "Usage: ./cortex_test <passes>" << endl;
    return 0;
  }
  int runs = atoi(argv[1]);

  foo f;
  bar b;
  cortex c;

  time_t iterative_sum = 0;
  time_t threaded_sum = 0;

  for(int passes=0; passes<runs; ++passes) {
    cout << "BENCHMARKS PASS " << passes+1 << endl;

    // ITERATIVE PASS
    c.set_process_flag(false);
    fill(c, f, b);
    time_t iterative_pass_start = time(NULL);
    c.process_gate_iteratively();
    time_t iterative_pass_end = time(NULL);
    iterative_sum += (iterative_pass_end - iterative_pass_start);

    // THREADED PASS
    c.set_process_flag(true);
    time_t threaded_pass_start = time(NULL);
    fill(c, f, b);
    c.wait_for_empty_queue();
    time_t threaded_pass_end = time(NULL);
    threaded_sum += (threaded_pass_end - threaded_pass_start);
  }

  cout << "--------------------------------------\n";
  cout << "Total Run Time:\t" << iterative_sum+threaded_sum << endl;
  cout << "Iterative Total:\t" << iterative_sum << endl;
  cout << "Iterative Avg:\t" << iterative_sum / runs << endl;
  cout << "Threaded Total:\t" << threaded_sum << endl;
  cout << "Threaded Avg:\t" << threaded_sum / runs << endl;

  return 0;
}
