// task.h

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
