#include <iostream>
#include "internals.h"
#include "cortex.h"
#include "boundjob.h"

using namespace std;
using namespace the_cortex;

#define ELEMENTS   125000
#define POINTLESSNESS 100
#define BENCHMARK true

namespace {
  void wasteTimeWithMath(double *mod, int num)
  {
    for (int i = 0; i < POINTLESSNESS; ++i) {
      *mod = *mod * num / num + num - num;
    }
  }
}
/**
 *   Polymorphic classes
 */
class object {
  public:
    double blah;
    virtual void func1() {
      wasteTimeWithMath(&blah, 2);
      if(!BENCHMARK) cout << "obj -- func1 -- blah = " << blah << endl;
    }
    virtual void func2() {
      wasteTimeWithMath(&blah, 3);
      if(!BENCHMARK) cout << "obj -- func2 -- blah = " << blah << endl;
    }
    void set_blah(double in) {
      blah = in;
    }
};

class foo : public object {
  public:
    void func1() {
      wasteTimeWithMath(&blah, 4);
      if(!BENCHMARK) cout << "foo -- func1 -- blah = " << blah << endl;
    }
    void func2() {
      wasteTimeWithMath(&blah, 5);
      if(!BENCHMARK) cout << "foo -- func2 -- blah = " << blah << endl;
    }
};

class bar : public object {
  public:
    void func1() {
      wasteTimeWithMath(&blah, 2);
      if(!BENCHMARK) cout << "bar -- func1 -- blah = " << blah << endl;
    }
    void func2() {
      wasteTimeWithMath(&blah, 2);
      if(!BENCHMARK) cout << "bar -- func2 -- blah = " << blah << endl;
    }
};

/**
 *   Thread functions
 */
void _threaded_a(object *arg) {
   arg->func1();
}

void _threaded_b(object *arg) {
   arg->func2();
}

/**
 *   fill()
 *
 *   pretty self explanatory
 */
void fill(Cortex &c, object *b, object *f) {
  for(size_t i=0; i<ELEMENTS; ++i)
  {
    c.enqueue_task(functional::BindUtil::bind(&_threaded_a, f));
    c.enqueue_task(functional::BindUtil::bind(&_threaded_b, f));
    c.enqueue_task(functional::BindUtil::bind(&_threaded_a, b));
    c.enqueue_task(functional::BindUtil::bind(&_threaded_b, b));
  }
}

/**
 *   main()
 */
int main(int argc, char ** argv) {

  int runs = 0;
  if(argc == 2) {
    runs = atoi(argv[1]);
  }
  else {
    runs = 1;
  }

  // c1 for iterative, c2 for threaded
  Cortex c1, c2;
  // start the threads; workers should automatically block
  c2.start();

  double iterative_sum = 0;
  double threaded_sum = 0;

  cout << "Running " << runs << " passes for "
      << 4*ELEMENTS << " elements..." << endl;
  for(int passes=0; passes<runs; ++passes) {
    // generate a new set of objects each iteration to prevent any compiler
    // optimizations
    foo f;
    f.set_blah(rand());
    bar b;
    b.set_blah(rand());
    cout << "Pass\t" << passes+1 << endl;

    // ITERATIVE PASS
    fill(c1, &b, &f);
    time_t iterative_pass_start = time(NULL);
    c1.process_gate_iteratively();
    time_t iterative_pass_end = time(NULL);
    double diff = difftime(iterative_pass_end, iterative_pass_start);
    iterative_sum += diff;
    cout << "\tIterative:\t" << (double)diff << " seconds" << endl;

    // THREADED PASS
    fill(c2, &b, &f);
    time_t threaded_pass_start = time(NULL);
    c2.drain();
    time_t threaded_pass_end = time(NULL);
    diff = difftime(threaded_pass_end, threaded_pass_start);
    threaded_sum += diff;
    cout << "\tThreaded:\t" << (double)diff << " seconds" << endl;
  }
  c2.stop();

  cout << "--------------------------------------\n";
  cout << "Total Run Time:\t\t" << iterative_sum+threaded_sum
      << " seconds" << endl;
  cout << "Iterative Total:\t" << iterative_sum
      << " seconds" << endl;
  cout << "Iterative Avg:\t\t" << iterative_sum / runs
      << " seconds" << endl;
  cout << "Threaded Total:\t\t" << threaded_sum
      << " seconds" << endl;
  cout << "Threaded Avg:\t\t"  << threaded_sum / runs
      << " seconds" << endl;
  return 0;
}
