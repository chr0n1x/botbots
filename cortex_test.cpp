#include <iostream>
#include "internals.hpp"
#include "cortex.hpp"

using namespace std;

class object {
  public:
    virtual void func1() { cout << "obj -- func1" << endl; }
    virtual void func2() { cout << "obj -- func2" << endl; }
};

class foo : public object {
  public:
    void func1() { cout << "foo -- func1" << endl; }
    void func2() { cout << "foo -- func2" << endl; }
};

class bar : public object {
  public:
    void func1() { cout << "bar -- func1" << endl; }
    void func2() { cout << "bar -- func2" << endl; }
};

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

  cortex<object> c;

  c.queue_task(&f, _threaded_a);
  c.queue_task(&f, _threaded_b);

  c.queue_task(&b, _threaded_a);
  c.queue_task(&b, _threaded_b);
}
