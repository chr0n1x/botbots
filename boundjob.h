// boundjob.h

// These class templates implement a function with bound argument(s).

#include "task.h"

namespace functional {

class BoundJob : public Task
{
    // PRIVATE TYPES
    typedef void(*Job)(void);

    // PRIVATE DATA
    Job d_job;

  public:

    // CREATORS

    BoundJob(Job job);

    BoundJob(const BoundJob& original);

    virtual ~BoundJob();

    // MANIPULATORS

    virtual void execute();
};

template <class ARG1>
class BoundJob1 : public Task
{
    // PRIVATE TYPES
    typedef void(*Job)(ARG1);

    // PRIVATE DATA
    Job d_job;
    ARG1 d_arg1;

  public:

    // CREATORS

    BoundJob1(Job job, ARG1 d_arg1);

    BoundJob1(const BoundJob1& original);

    virtual ~BoundJob1();

    // MANIPULATORS

    virtual void execute();
};

template <class ARG1, class ARG2>
class BoundJob2 : public Task
{
    // PRIVATE TYPES
    typedef void(*Job)(ARG1, ARG2);

    // PRIVATE DATA
    Job d_job;
    ARG1 d_arg1;
    ARG2 d_arg2;

  public:

    // CREATORS

    BoundJob2(Job job, ARG1 d_arg1, ARG2 d_arg2);

    BoundJob2(const BoundJob2& original);

    virtual ~BoundJob2();

    // MANIPULATORS

    virtual void execute();
};



//============================================================================
//                          INLINE DEFINITIONS
//============================================================================

// NO ARGS

BoundJob::BoundJob(Job job)
: d_job(job)
{
}

BoundJob::BoundJob(const BoundJob& original)
: d_job(original.d_job)
{
}

BoundJob::~BoundJob()
{
}

void BoundJob::execute()
{
    if(d_job) {
        (*d_job)();
    }
}

// ONE ARGS

template <class ARG1>
BoundJob1<ARG1>::BoundJob1(Job job, ARG1 arg1)
: d_job(job)
, d_arg1(arg1)
{
}

template <class ARG1>
BoundJob1<ARG1>::BoundJob1(const BoundJob1<ARG1>& original)
: d_job(original.d_job)
, d_arg1(original.arg1)
{
}

template <class ARG1>
BoundJob1<ARG1>::~BoundJob1()
{
}

template <class ARG1>
void BoundJob1<ARG1>::execute()
{
    if(d_job) {
        (*d_job)(d_arg1);
    }
}

// TWO ARGS

template <class ARG1, class ARG2>
BoundJob2<ARG1, ARG2>::BoundJob2(Job job, ARG1 arg1, ARG2 arg2)
: d_job(job)
, d_arg1(arg1)
, d_arg2(arg2)
{
}

template <class ARG1, class ARG2>
BoundJob2<ARG1, ARG2>::BoundJob2(const BoundJob2<ARG1, ARG2>& original)
: d_job(original.d_job)
, d_arg1(original.d_arg1)
, d_arg2(original.d_arg2)
{
}

template <class ARG1, class ARG2>
BoundJob2<ARG1, ARG2>::~BoundJob2()
{
}

template <class ARG1, class ARG2>
void BoundJob2<ARG1, ARG2>::execute()
{
    if(d_job) {
        (*d_job)(d_arg1, d_arg2);
    }
}

}  // closing namespace functional
