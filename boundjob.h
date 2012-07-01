// boundjob.h

#ifndef INCLUDED_BOUNDJOB_H
#define INCLUDED_BOUNDJOB_H

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

template <class ARG1, class ARG2, class ARG3>
class BoundJob3 : public Task
{
    // PRIVATE TYPES
    typedef void(*Job)(ARG1, ARG2);

    // PRIVATE DATA
    Job d_job;
    ARG1 d_arg1;
    ARG2 d_arg2;
    ARG3 d_arg3;

  public:

    // CREATORS

    BoundJob3(Job job, ARG1 d_arg1, ARG2 d_arg2, ARG3 d_arg3);

    BoundJob3(const BoundJob3& original);

    virtual ~BoundJob3();

    // MANIPULATORS

    virtual void execute();
};


struct BindUtil
{
    static Task *bind(void(*job)(void));

    template <class ARG1>
    static Task *bind(void(*job)(ARG1), ARG1 arg1);

    template <class ARG1, class ARG2>
    static Task *bind(void(*job)(ARG1, ARG2), ARG1 arg1, ARG2 arg2);

    template <class ARG1, class ARG2, class ARG3>
    static Task *bind(void(*job)(ARG1, ARG2, ARG3),
                      ARG1 arg1, ARG2 arg2, ARG3 arg3);
};

//============================================================================
//                          INLINE DEFINITIONS
//============================================================================

// NO ARGS

inline
BoundJob::BoundJob(Job job)
: d_job(job)
{
}

inline
BoundJob::BoundJob(const BoundJob& original)
: d_job(original.d_job)
{
}

inline
BoundJob::~BoundJob()
{
}

inline
void BoundJob::execute()
{
    if(d_job) {
        (*d_job)();
    }
}

// ONE ARGS

template <class ARG1>
inline
BoundJob1<ARG1>::BoundJob1(Job job, ARG1 arg1)
: d_job(job)
, d_arg1(arg1)
{
}

template <class ARG1>
inline
BoundJob1<ARG1>::BoundJob1(const BoundJob1<ARG1>& original)
: d_job(original.d_job)
, d_arg1(original.d_arg1)
{
}

template <class ARG1>
inline
BoundJob1<ARG1>::~BoundJob1()
{
}

template <class ARG1>
inline
void BoundJob1<ARG1>::execute()
{
    if(d_job) {
        (*d_job)(d_arg1);
    }
}

// TWO ARGS

template <class ARG1, class ARG2>
inline
BoundJob2<ARG1, ARG2>::BoundJob2(Job job, ARG1 arg1, ARG2 arg2)
: d_job(job)
, d_arg1(arg1)
, d_arg2(arg2)
{
}

template <class ARG1, class ARG2>
inline
BoundJob2<ARG1, ARG2>::BoundJob2(const BoundJob2<ARG1, ARG2>& original)
: d_job(original.d_job)
, d_arg1(original.d_arg1)
, d_arg2(original.d_arg2)
{
}

template <class ARG1, class ARG2>
inline
BoundJob2<ARG1, ARG2>::~BoundJob2()
{
}

template <class ARG1, class ARG2>
inline
void BoundJob2<ARG1, ARG2>::execute()
{
    if(d_job) {
        (*d_job)(d_arg1, d_arg2);
    }
}

// THREE ARGS

template <class ARG1, class ARG2, class ARG3>
inline
BoundJob3<ARG1, ARG2, ARG3>::BoundJob3(Job job, ARG1 arg1, ARG2 arg2, ARG3 arg3)
: d_job(job)
, d_arg1(arg1)
, d_arg2(arg2)
, d_arg3(arg3)
{
}

template <class ARG1, class ARG2, class ARG3>
inline
BoundJob3<ARG1, ARG2, ARG3>::BoundJob3(
        const BoundJob3<ARG1, ARG2, ARG3>& original)
: d_job(original.d_job)
, d_arg1(original.d_arg1)
, d_arg2(original.d_arg2)
, d_arg3(original.d_arg3)
{
}

template <class ARG1, class ARG2, class ARG3>
inline
BoundJob3<ARG1, ARG2, ARG3>::~BoundJob3()
{
}

template <class ARG1, class ARG2, class ARG3>
inline
void BoundJob3<ARG1, ARG2, ARG3>::execute()
{
    if(d_job) {
        (*d_job)(d_arg1, d_arg2, d_arg3);
    }
}
                            //===============
                            // class BindUtil
                            //===============
inline
Task *BindUtil::bind(void(*job)(void))
{
    return new BoundJob(job);
}

template <class ARG1>
inline
Task *BindUtil::bind(void(*job)(ARG1), ARG1 arg1)
{
    return new BoundJob1<ARG1>(job, arg1);
}

template <class ARG1, class ARG2>
inline
Task *BindUtil::bind(void(*job)(ARG1, ARG2), ARG1 arg1, ARG2 arg2)
{
    return new BoundJob2<ARG1, ARG2>(job, arg1, arg2);
}

template <class ARG1, class ARG2, class ARG3>
inline
Task *BindUtil::bind(void(*job)(ARG1, ARG2, ARG3),
                     ARG1 arg1, ARG2 arg2, ARG3 arg3)
{
    return new BoundJob3<ARG1, ARG2, ARG3>(job, arg1, arg2, arg3);
}

}  // closing namespace functional

#endif
