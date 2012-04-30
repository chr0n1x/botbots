#include "grid.hpp"

#include <iostream>
#include <cstdlib>

#include <ncurses.h>
#include <unistd.h>

using namespace std;

int main(int argc, char ** argv) {

  int grid_dim = 8;
  if(argc > 1) {
    grid_dim = atoi(argv[1]);
  }
  grid::the_grid g(grid_dim);

  initscr();

  g.fill_to_capacity();
  while(g.botbot_count() > 0) {
    erase();
    g.initiate_cycle();
    printw(g.to_string().c_str());
    refresh();
    sleep(1);
  }

  endwin();

  return 0;
}
