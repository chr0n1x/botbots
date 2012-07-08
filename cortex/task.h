#ifndef __TASK
#define __TASK

#include <cstdlib>

namespace functional {

  // forward declarations
  //class ArgumentObject;

  class ArgumentObject {
    protected:
      ArgumentObject * absolute_pointer;

    public:
      ArgumentObject();
      virtual ~ArgumentObject();
      template <class _TYPE> void bindArgument(_TYPE arg);

      virtual ArgumentObject *absolutePointer();
  };

  template <class _TYPE>
  class SpecArgumentObject : public ArgumentObject {
    _TYPE data;

    public:
      //SpecArgumentObject();
      SpecArgumentObject(_TYPE);
      virtual ~SpecArgumentObject();

      ArgumentObject * absolutePointer();
  };

  /*
  template <typename FUNC_TYPE, 
  class Task {
    public:
      Task() {}
      ~Task() {}
  };

  template <class ARG_TYPE>
  static struct BindUtil {
    Task bind(void(*func)(void), ARG_TYPE obj);
  };
  */
}

#endif
