// task.h

#ifndef INCLUDED_TASK_H
#define INCLUDED_TASK_H

// This class defines a protocol for a function that is bound with zero or more
// arguments.

namespace functional {

class Task
{
    // NOT IMPLEMENTED

  public:

    virtual ~Task();

    virtual void execute() = 0;
};

}

#endif
