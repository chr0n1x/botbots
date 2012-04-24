#include "grid.hpp"

#include <iostream>
using namespace std;

int main(void) {
  grid::the_grid g;
  cout << "Grid cells: " << g.cell_count() << endl;

  int count = 0;
  while(g.create_botizen()) {
    ++count;
  }

  g.print_to_console();
  return 0;
}
