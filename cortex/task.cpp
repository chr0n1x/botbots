#include <iostream>
#include "task.h"

namespace functional {

  /**
   *  ArgumentObject CLASS
   */

  ArgumentObject::ArgumentObject() : absolute_pointer(NULL) {}

  // assume assignment operator is already defined for deep copies
  // if user defined class
  template <class type>
  void ArgumentObject::bindArgument(type arg) {
    SpecArgumentObject<type> sao(arg);
    // coerce or implicit?
    absolute_pointer = sao.absolutePointer();
  }

  ArgumentObject * ArgumentObject::absolutePointer() {
    return absolute_pointer;
  }

  /**
   *  SpecArgumentObject CLASS
   */

  // only allow explicit for now
  /*
  template <class type>
  SpecArgumentObject::SpecArgumentObject() {

  }
  */

  template <class _TYPE>
  SpecArgumentObject<_TYPE>::SpecArgumentObject(_TYPE arg) {
    this->data = arg;
    absolute_pointer = this;
  }

  /*
  Task::Task() {

  }

  Task::~Task() {

  }
  */

}
