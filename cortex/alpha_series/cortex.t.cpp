#include "boundjob.h"
#include "cortex.h"
#include "multiqueue_threadpool.h"
#include "timer.h"

using namespace the_cortex;

#include <iostream>
#include <string>
#include <map>
#include <set>

#include <cstdlib>

using namespace std;

#define POINTLESSNESS 10000
#define BENCHMARK     true

int ELEMENTS = 100000;

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

void fill(MultiQueueThreadpool &mqt, object *b, object *f) {
  for(size_t i=0; i<ELEMENTS; ++i)
  {
    mqt.enqueue_task(functional::BindUtil::bind(&_threaded_a, f));
    mqt.enqueue_task(functional::BindUtil::bind(&_threaded_b, f));
    mqt.enqueue_task(functional::BindUtil::bind(&_threaded_a, b));
    mqt.enqueue_task(functional::BindUtil::bind(&_threaded_b, b));
  }
}

void process_in_thread(object *b, object *f)
{
  for(size_t i=0; i<ELEMENTS; ++i)
  {
    _threaded_a(f);
    _threaded_b(f);
    _threaded_a(b);
    _threaded_b(b);
  }
}

struct WorkerData {
    int num_iter;
    object *b;
    object *f;
};

void *raw_worker(void *data)
{
    WorkerData *work = (WorkerData*)data;
    for (int i = 0; i < work->num_iter; ++i)
    {
        _threaded_a(work->f);
        _threaded_b(work->f);
        _threaded_a(work->b);
        _threaded_b(work->b);
    }
    return NULL;
}

void runRawThreadTest(int numThreads, object *b, object *f)
{
    std::vector<pthread_t>  workers(numThreads);
    int iter_per_thread = ELEMENTS / numThreads;
    WorkerData data = {iter_per_thread, b, f};
    // this one thread may have to work a bit harder, not balanced...
    WorkerData leftover = {(ELEMENTS - (iter_per_thread*numThreads-1)), b, f};
    // So lets start him first
    pthread_create(&workers[0], NULL, raw_worker, (void*)&leftover);
    for(int i = 1; i < workers.size(); ++i)
    {
        pthread_create(&workers[i], NULL, raw_worker, (void*)&data);
    }

    for(int i=0; i < workers.size(); ++i)
    {
        pthread_join(workers[i], NULL);
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
    bool elements_assigned = false;
    bool flags_found = false;

    for(int i=1; i<argc; ++i) {
      dummy = atoi(argv[i]);

      // has a dash so...non numeric
      if(dummy == 0) {
        string option(argv[i]);
        int dash_pos = option.find("-", 0);
        while(dash_pos != string::npos) {
            option.erase(dash_pos, 1);
            dash_pos = option.find("-", 0);
        }

        if( opts.find(option) != opts.end() ) {
          opts[option] = 1;
          flags_found = true;
        }
        else {
          unknown_options.insert(option);
        }
      }
      // numeric...?
      else {
        if(!runs_assigned) {
          opts["runs"] = atoi(argv[i]);
          runs_assigned = true;
        }
        else if(!elements_assigned) {
          ELEMENTS = atoi(argv[i]);
          elements_assigned = true;
        }
        else {
          runs_assigned = true;
          opts["threads"] = atoi(argv[i]);
        }
      }
    }

    if(unknown_options.empty()) {

      if(opts.find("help")->second) {
        cout << "Usage:\t" << argv[0]
             << " [n_runs] [n_elements] [n_threads] [flags]" << endl;
        cout << "Note:\tcmd line input is optional; "
                "order of numerical values matter; n_elements multiplied by 4"
             << endl;
        return false;
      }

      cout << "Running " << opts["runs"]
           << " passes for " << 4*ELEMENTS << " elements ";
      cout << "with " << opts["threads"] << " threads;" << endl;

      if(flags_found) {
        cout << "Option(s):" << endl;
        map<string, int>::iterator map_it = opts.begin();
        for(map_it; map_it != opts.end(); ++map_it) {
          if(map_it->second && map_it->first != "runs"
          && map_it->first != "threads")
            cout << "\t" << map_it->first << endl;
        }
      }

    }
    else {
      cout << "Unknown Option(s):" << endl;
      set<string>::iterator set_it = unknown_options.begin();
      for(set_it; set_it != unknown_options.end(); ++set_it)
        cout << "\t" << *set_it << endl;
      cout << "Try:\t" << argv[0] << " -help" << endl;
      return false;
    }
  }
  else {
    cout << "Running " << opts["runs"]
         << " passes for " << 4*ELEMENTS << " elements ";
    cout << "with " << opts["threads"] << " threads;" << endl;
  }

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
  opts.insert( opts_t("help", 0) );

  if(!parseOptions(argc, argv, opts))
    return 0;
  int threads = opts["threads"];
  int runs = opts["runs"];
  bool with_iterative = (opts.find("threads_only") != opts.end() && opts.find("threads_only")->second == 0);

  double iterative_sum = 0;
  double threaded_sum = 0;
  double mqt_sum = 0;
  double raw_threaded_sum = 0;
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
      prof::Timer it_timer;
      it_timer.start();
      process_in_thread(&b, &f);
      it_timer.stop();
      totalLinearTime = it_timer.elapsed_time();
      iterative_sum += totalLinearTime;
      cout << "\tIterative:\t" << totalLinearTime << " seconds" << endl;
    }

    // THREADED PASS
    prof::Timer thr_timer;
    thr_timer.start();
    Cortex threadpool(threads);
    threadpool.start();

    fill(threadpool, &b, &f);
    threadpool.drain();

    threadpool.stop();
    thr_timer.stop();
    totalLinearTime = thr_timer.elapsed_time();
    threaded_sum += totalLinearTime;
    cout << "\tThreaded:\t" << (double)totalLinearTime << " seconds" << endl;

    // multiqueue_threadpool pass
    prof::Timer mqt_timer;
    mqt_timer.start();
    MultiQueueThreadpool mqt_tp(threads);
    mqt_tp.start();

    fill(mqt_tp, &b, &f);
    mqt_tp.drain();

    mqt_tp.stop();
    mqt_timer.stop();
    totalLinearTime = mqt_timer.elapsed_time();
    mqt_sum += totalLinearTime;
    cout << "\tMQT Threaded:\t" << (double)totalLinearTime
                                << " seconds" << endl;

    // Raw threads
    prof::Timer raw_timer;
    raw_timer.start();
    runRawThreadTest(threads, &b, &f);
    raw_timer.stop();
    totalLinearTime = raw_timer.elapsed_time();
    raw_threaded_sum += totalLinearTime;
    cout << "\tRaw Threaded:\t" << (double)totalLinearTime
         << " seconds" << endl;
  }

  cout << "--------------------------------------\n";
  cout << "Total Run Time:\t\t\t" << iterative_sum+threaded_sum
      << " seconds" << endl;

  if(with_iterative) {
    cout << "Iterative Total/Avg:\t" << iterative_sum << " / "
         << iterative_sum / runs << " seconds" << endl;
  }

  cout << "Threaded Total/Avg:\t\t" << threaded_sum << " / "
       << threaded_sum / runs << " seconds" << endl;
  cout << "MQT Threaded Total/Avg:\t\t" << mqt_sum << " / "
       << mqt_sum / runs << " seconds" << endl;
  cout << "Raw Threaded Total/Avg:\t" << raw_threaded_sum << " / "
       << raw_threaded_sum / runs << " seconds" << endl;

  // print speedup
  if(with_iterative) {
    double speedup = (iterative_sum / threaded_sum);
    cout << "Speedup:\t\t\t\t" << speedup << "x" << endl;
    cout << "Raw Speedup:\t\t\t" << iterative_sum / raw_threaded_sum
         << "x" << endl;
    cout << "Cortex overhead:\t\t"
        << (threaded_sum - raw_threaded_sum) / raw_threaded_sum * 100
        << "%" << endl;
    cout << "Optimal:\t\t\t\t" << threads << "x" << endl;
  }

  return 0;
}
