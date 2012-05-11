#include <iostream>
#include "internals.hpp"
#include "cortex.hpp"

using namespace std;
using namespace the_cortex;

#define ELEMENTS  125000
#define BENCHMARK true

/**
 *  Polymorphic classes
 */
class object {
  double blah;
  public:
    virtual void func1() { if(!BENCHMARK) cout << "obj -- func1"; }
    virtual void func2() { if(!BENCHMARK) cout << "obj -- func2"; }
    void set_blah(double in) {
      blah = in;
    }
};

class foo : public object {
  public:
    void func1() { if(!BENCHMARK) cout << "foo -- func1"; }
    void func2() { if(!BENCHMARK) cout << "foo -- func2"; }
};

class bar : public object {
  public:
    void func1() { if(!BENCHMARK) cout << "bar -- func1"; }
    void func2() { if(!BENCHMARK) cout << "bar -- func2"; }
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

/**
 *  fill()
 *
 *  pretty self explanatory
 */
void fill(cortex &c, bar b, foo f) {
  for(size_t i=0; i<ELEMENTS; ++i) {
    c.queue_task(&f, &_threaded_a);
    c.queue_task(&f, &_threaded_b);
    c.queue_task(&b, &_threaded_a);
    c.queue_task(&b, &_threaded_b);
  }
}

/**
 *  main()
 */
int main(int argc, char ** argv) {

  int runs = 0;
  if(argc == 2) {
    runs = atoi(argv[1]);
  }
  else {
    runs = 1;
  }

  cortex c1, c2;
  c1.set_process_flag(false);

  double iterative_sum = 0;
  double threaded_sum = 0;

  cout << "Running " << runs << " passes for " << 4*ELEMENTS << " elements..." << endl;
  for(int passes=0; passes<runs; ++passes) {
    // generate a new set of objects each iteration to prevent any compiler optimizations
    foo f;
    f.set_blah(rand());
    bar b;
    b.set_blah(rand());
    cout << "Pass\t" << passes+1 << endl;

    // ITERATIVE PASS
    time_t iterative_pass_start = time(NULL);
    fill(c1, b, f);
    c1.process_gate_iteratively();
    time_t iterative_pass_end = time(NULL);
    double diff = difftime(iterative_pass_end, iterative_pass_start);
    iterative_sum += diff;
    cout << "\tIterative:\t" << (double)diff << " seconds" << endl;

    // THREADED PASS
    time_t threaded_pass_start = time(NULL);
    fill(c2, b, f);
    c2.wait_for_empty_queue((passes == runs-1));
    time_t threaded_pass_end = time(NULL);
    diff = difftime(threaded_pass_end, threaded_pass_start);
    threaded_sum += diff;
    cout << "\tThreaded:\t" << (double)diff << " seconds" << endl;
  }

  cout << "--------------------------------------\n";
  cout << "Total Run Time:\t\t" << iterative_sum+threaded_sum  << " seconds" << endl;
  cout << "Iterative Total:\t"  << iterative_sum               << " seconds" << endl;
  cout << "Iterative Avg:\t\t"  << iterative_sum / runs        << " seconds" << endl;
  cout << "Threaded Total:\t\t" << threaded_sum                << " seconds" << endl;
  cout << "Threaded Avg:\t\t"   << threaded_sum / runs         << " seconds" << endl;

  return 0;
}
