#include "boundjob.h"
#include "cortex.h"
#include "multiqueue_threadpool.h"
#include "timer.h"

using namespace the_cortex;
using namespace functional;

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <cstdlib>
#include <ctime>

using namespace std;

#define P(val) cout << #val << " = " << val << " "

namespace {
    void wasteTimeWithMath(int numIterations)
    {
        double dummy1, dummy2;
        for (int i = 0; i < numIterations; ++i)
        {
            dummy1 = dummy2*dummy1+dummy2/dummy1-dummy2*dummy1;
        }
    }

    void genAndSort(int numElements)
    {
        vector<int> randElem(numElements);
        for(int i = 0; i < numElements; ++i)
        {
            randElem[i] = i;
        }
        random_shuffle(randElem.begin(), randElem.end());
        sort(randElem.begin(), randElem.end());
    }
}

void fill(Cortex &c, const vector<int>& numbers)
{
    for(size_t i = 0; i < numbers.size(); ++i)
    {
        //c.enqueue_task(BindUtil::bind(&wasteTimeWithMath, numbers[i]));
        c.enqueue_task(BindUtil::bind(&genAndSort, numbers[i]));
    }
}

void fill(MultiQueueThreadpool &mqt, const vector<int>& numbers)
{
    for(size_t i = 0; i < numbers.size(); ++i)
    {
        //mqt.enqueue_task(BindUtil::bind(&wasteTimeWithMath, numbers[i]));
        mqt.enqueue_task(BindUtil::bind(&genAndSort, numbers[i]));
    }
}

void process_in_thread(const vector<int>& numbers)
{
    for(size_t i = 0; i < numbers.size(); ++i)
    {
        //wasteTimeWithMath(numbers[i]);
        genAndSort(numbers[i]);
    }
}

struct WorkerData
{
    int start, end;
    const vector<int>& numbers;
};

void *raw_worker(void *data)
{
    WorkerData *work = (WorkerData*)data;
    for (int i = work->start; i < work->end; ++i)
    {
        //wasteTimeWithMath(work->numbers[i]);
        genAndSort(work->numbers[i]);
    }
    return NULL;
}

void runRawThreadTest(int numThreads, const vector<int>& numbers)
{
    std::vector<pthread_t>    workers(numThreads);
    int num_elements    = numbers.size();
    int iter_per_thread = num_elements / numThreads;
    int extra_load      = num_elements - (iter_per_thread*(numThreads-1));
    int start           = extra_load;
    // this one thread may have to work a bit harder, not balanced...
    WorkerData leftover = {0, extra_load, numbers};
    // So lets start him first
    pthread_create(&workers[0], NULL, raw_worker, (void*)&leftover);
    for(int i = 1; i < workers.size(); ++i)
    {
        WorkerData data = {start, start + iter_per_thread, numbers};
        pthread_create(&workers[i], NULL, raw_worker, (void*)&data);
        start += iter_per_thread;
    }

    for(int i=0; i < workers.size(); ++i)
    {
        pthread_join(workers[i], NULL);
    }
}


/**
 *     parseOptions()
 *
 *     Takes argc, argv and an option map from main() Goes through the options
 *     passed in.
 *
 *     returns false if there are unrecognized options in argv
 */
bool parseOptions(int argc, char ** argv, map<string, int> &opts)
{
    bool showHelp = false;
    if(argc > 1) {
        for(int i=1; i<argc; ++i)
        {
            string option(argv[i]);

            while (option[0] == '-') {
                option.erase(0, 1);
            }

            if( opts.find(option) != opts.end() && i < argc-1) {
                int val = atoi(argv[++i]);
                opts[option] = val;
            }
            else {
                cout << "Unknown option: " << option << endl;
                showHelp = true;
            }
        }

        if(showHelp || opts.find("help")->second) {
            cout << "Usage:\t" << argv[0] << " [flags]" << endl
                 << " --threads_only    Don't run an iterative pass" << endl
                 << " --runs            Number of passes"            << endl
                 << " --threads         Number of threads per pool"  << endl
                 << " --help            Display this text"           << endl
                 << " --elements        Number of jobs per pass"     << endl
                 << " --range_max       Maximum complexity per task" << endl
                 << " --sort            Sort work complexities"      << endl
                 << "All arguments take a single integer. "
                 << "0=Off for boolean arguments"                    << endl;
            return false;
        }

        cout << "Running "     << opts["runs"]
             << " passes for " << opts["elements"] << " elements "
             << "with "        << opts["threads"]  << " threads;" << endl;
    }

    return true;
}

/**
 *     main()
 */
int main(int argc, char ** argv)
{
    srand(time(NULL));
    // setting up flags and options
    map<string, int> opts;
    typedef pair<string, int> opts_t;
    opts.insert( opts_t("threads_only", 0) );
    opts.insert( opts_t("runs", 1) );
    opts.insert( opts_t("threads", 2) );
    opts.insert( opts_t("help", 0) );
    opts.insert( opts_t("elements", 10000) );
    opts.insert( opts_t("range_max", 10000) );
    opts.insert( opts_t("sort", 0) );

    if(!parseOptions(argc, argv, opts)) {
        return 0;
    }

    int threads         = opts.find("threads")->second;
    int runs            = opts.find("runs")->second;
    bool with_iterative = opts.find("threads_only")->second == 0;
    int range_max       = opts.find("range_max")->second;
    int num_elements    = opts.find("elements")->second;

    double iterative_sum = 0;
    double threaded_sum = 0;
    double mqt_sum = 0;
    double raw_threaded_sum = 0;
    double totalLinearTime;

    // generate the set of numbers to work off of
    unsigned long long averageComplexity = 0;
    vector<int> complexities(num_elements);
    for (int i = 0; i < complexities.size(); ++i)
    {
        complexities[i] = rand() % range_max + 1;
        averageComplexity += complexities[i];
    }

    cout << "Average work value = "
         << averageComplexity / num_elements << endl;

    if (opts.find("sort")->second != 0) {
        sort(complexities.begin(), complexities.end());
    }

    for(int passes=0; passes<runs; ++passes)
    {
        cout << "Pass\t" << passes+1 << endl;

        if(with_iterative) {
            // ITERATIVE PASS
            prof::Timer it_timer;
            it_timer.start();
            process_in_thread(complexities);
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

        fill(threadpool, complexities);
        threadpool.drain();

        threadpool.stop();
        thr_timer.stop();
        totalLinearTime = thr_timer.elapsed_time();
        threaded_sum += totalLinearTime;
        cout << "\tThreaded:\t" << (double)totalLinearTime
             << " seconds" << endl;

        // multiqueue_threadpool pass
        prof::Timer mqt_timer;
        mqt_timer.start();
        MultiQueueThreadpool mqt_tp(threads);
        mqt_tp.start();

        fill(mqt_tp, complexities);
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
        runRawThreadTest(threads, complexities);
        raw_timer.stop();
        totalLinearTime = raw_timer.elapsed_time();
        raw_threaded_sum += totalLinearTime;
        cout << "\tRaw Threaded:\t" << (double)totalLinearTime
                 << " seconds" << endl;
    }

    cout << "--------------------------------------\n";
    cout << "Total Run Time:\t\t" << iterative_sum+threaded_sum
            << " seconds" << endl;

    if(with_iterative) {
        cout << "Iterative Total/Avg:\t" << iterative_sum << " / "
                 << iterative_sum / runs << " seconds" << endl;
    }

    cout << "Threaded Total/Avg:\t" << threaded_sum << " / "
             << threaded_sum / runs << " seconds" << endl;
    cout << "MQT Threaded Total/Avg:\t" << mqt_sum << " / "
             << mqt_sum / runs << " seconds" << endl;
    cout << "Raw Threaded Total/Avg:\t" << raw_threaded_sum << " / "
             << raw_threaded_sum / runs << " seconds" << endl;

    // print speedup
    if(with_iterative) {
        double speedup = (iterative_sum / threaded_sum);
        cout << "Speedup:\t\t" << speedup << "x" << endl;
        cout << "Raw Speedup:\t\t" << iterative_sum / raw_threaded_sum
             << "x" << endl;
    }
    cout << "Cortex overhead:\t"
         << (threaded_sum - raw_threaded_sum) / raw_threaded_sum * 100
         << "%" << endl;
    cout << "Optimal:\t\t" << threads << "x" << endl;


    return 0;
}
