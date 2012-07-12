#include "internals.h"
#include "boundjob.h"
#include "cortex.h"
using namespace the_cortex;

#include <iostream>
#include <string>
#include <map>
#include <set>
using namespace std;

#include <time.h>
#include <sys/time.h>

#define ELEMENTS      500000 //250000 //125000
#define POINTLESSNESS 100
#define BENCHMARK     true

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
 *   parseOptions()
 *
 *   Takes argc, argv and an option map from main()
 *   Goes through the options passed in. The first numerical value given will be the number of runs
 *   Any numerical value after that will be the number of threads for the cortexes.
 *
 *   returns false if there are unrecognized options in argv
 */
bool parseOptions(int argc, char ** argv, map<string, int> &opts) {
  if(argc > 1) {
    set<string> unknown_options;

    int dummy = 0;
    bool runs_assigned = false;
    bool flags_found = false;

    for(int i=1; i<argc; ++i) {
      dummy = atoi(argv[i]);

      if(dummy == 0) {
        string option(argv[i]);
        int dash_pos = option.find("-", 0);
        while(dash_pos != string::npos) {
            option.erase(dash_pos, 1);
            dash_pos = option.find("-", 0);
        }

        if( opts.find(option) != opts.end() ) {
          opts[option] = true;
          flags_found = true;
        }
        else {
          unknown_options.insert(option);
        }
      }
      else {
        if(runs_assigned) {
          opts["threads"] = atoi(argv[i]);
        }
        else {
          opts["runs"] = atoi(argv[i]);
          runs_assigned = true;
        }
      }
    }

    if(unknown_options.empty()) {
      cout << "Running " << opts["runs"] << " passes for " << 4*ELEMENTS << " elements ";
      cout << "with " << opts["threads"] << " threads;" << endl;

      if(flags_found) {
        cout << "Option(s):" << endl;
        map<string, int>::iterator map_it = opts.begin();
        for(map_it; map_it != opts.end(); ++map_it) {
          if(map_it->second && map_it->first.compare("runs") != 0 && map_it->first.compare("threads") != 0)
            cout << "\t" << map_it->first << endl;
        }
      }

    }
    else {
      cout << "Unknown Option(s):" << endl;
      set<string>::iterator set_it = unknown_options.begin();
      for(set_it; set_it != unknown_options.end(); ++set_it)
        cout << "\t" << *set_it << endl;
      cout << "Aborting..." << endl;
      return false;
    }
  }
  else {
    cout << "Running " << opts["runs"] << " passes for " << 4*ELEMENTS << " elements ";
    cout << "with " << opts["threads"] << " threads;" << endl;
  }

  cout << endl;

  return true;
}

/**
 *   main()
 */
int main(int argc, char ** argv) {

  // setting up flags and options
  map<string, int> opts;
  typedef pair<string, int> opts_t;
  opts.insert( opts_t("threads_only", 0) );
  opts.insert( opts_t("runs", 1) );
  opts.insert( opts_t("threads", 2) );

  if(!parseOptions(argc, argv, opts))
    return 0;
  int threads = opts["threads"];
  int runs = opts["runs"];
  bool with_iterative = (opts.find("threads_only") != opts.end() && opts.find("threads_only")->second == 0);

  // c1 for iterative, c2 for threaded
  Cortex c1(threads), c2(threads);
  // start the threads; workers should automatically block
  c2.start();

  struct timeval startLinear, finishLinear;
  double iterative_sum = 0;
  double threaded_sum = 0;
  double totalLinearTime;

  for(int passes=0; passes<runs; ++passes) {
    // generate a new set of objects each iteration to prevent any compiler
    // optimizations
    foo f;
    f.set_blah(rand());
    bar b;
    b.set_blah(rand());
    cout << "Pass\t" << passes+1 << endl;

    if(with_iterative) {
      // ITERATIVE PASS
      gettimeofday(&startLinear, NULL);
      fill(c1, &b, &f);
      c1.process_gate_iteratively();
      gettimeofday(&finishLinear, NULL);
      totalLinearTime = 
        (double)((double)(finishLinear.tv_sec - startLinear.tv_sec) * 1000000 + 
        (double)(finishLinear.tv_usec - startLinear.tv_usec)) / 
        (double)1000000;
      iterative_sum += totalLinearTime;
      cout << "\tIterative:\t" << (double)totalLinearTime << " seconds" << endl;
    }

    // THREADED PASS
    gettimeofday(&startLinear, NULL);
    fill(c2, &b, &f);
    c2.drain();
    gettimeofday(&finishLinear, NULL);
    totalLinearTime = 
      (double)((double)(finishLinear.tv_sec - startLinear.tv_sec) * 1000000 + 
      (double)(finishLinear.tv_usec - startLinear.tv_usec)) / 
      (double)1000000;
    threaded_sum += totalLinearTime;
    cout << "\tThreaded:\t" << (double)totalLinearTime << " seconds" << endl;
  }
  c2.stop();

  cout << "--------------------------------------\n";
  cout << "Total Run Time:\t\t" << iterative_sum+threaded_sum
      << " seconds" << endl;

  if(with_iterative) {
    cout << "Iterative Total:\t" << iterative_sum
        << " seconds" << endl;
    cout << "Iterative Avg:\t\t" << iterative_sum / runs
        << " seconds" << endl;
  }

  cout << "Threaded Total:\t\t" << threaded_sum
      << " seconds" << endl;
  cout << "Threaded Avg:\t\t"  << threaded_sum / runs
      << " seconds" << endl;
  return 0;
}
