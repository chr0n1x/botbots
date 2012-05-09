#include <iostream>
#include "internals.hpp"
#include "cortex.hpp"

using namespace std;

/**
 *  Polymorphic classes
 */
class object {
  public:
    virtual void func1() { cout << "obj -- func1\n"; }
    virtual void func2() { cout << "obj -- func2"; }
};

class foo : public object {
  public:
    void func1() { cout << "foo -- func1\n"; }
    void func2() { cout << "foo -- func2\n"; }
};

class bar : public object {
  public:
    void func1() { cout << "bar -- func1\n"; }
    void func2() { cout << "bar -- func2\n"; }
};

/**
 *  Thread functions
 */
void *_threaded_a(void *arg) {
  object * obj = (object *) arg;
  obj->func1();
}

void *_threaded_b(void *arg) {
  object * obj = (object *) arg;
  obj->func2();
}

int main() {
  using namespace the_cortex;

  foo f;
  bar b;

  cortex c;
  // hold off on processing for now
  c.set_process_flag(false);

  for(int i=0; i<10000; ++i) {
    c.queue_task(&f, &_threaded_a);
    c.queue_task(&f, &_threaded_b);
    c.queue_task(&b, &_threaded_a);
    c.queue_task(&b, &_threaded_b);
  }

  cout << "--------------------------------------\n";

  c.set_process_flag(true);
  c.wait_for_empty_queue();

  return 0;
}
