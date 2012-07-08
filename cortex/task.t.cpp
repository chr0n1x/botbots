#include <iostream>
#include <typeinfo>
using namespace std;

#include "task.h"
using namespace functional;

int main(int argc, char ** argv) {

  SpecArgumentObject<int> obj1(5);
  ArgumentObject * ptr = &obj1;

  cout << typeid(obj1).name() << endl;
  cout << typeid(*ptr).name() << endl;

  return 0;
}
