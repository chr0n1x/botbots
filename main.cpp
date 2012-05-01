#include "grid.hpp"

#include <iostream>
#include <cstdlib>

#include <ncurses.h>
//#include <unistd.h>

using namespace std;

int main(int argc, char ** argv) {

  int grid_rows = 0;
  int grid_cols = 0;

  if(argc == 3) {
    grid_rows = atoi(argv[1]);
    grid_cols = atoi(argv[2]);
  }
  grid::the_grid g(grid_rows, grid_cols);
  cout << "Initializing Grid..." << endl;
  g.fill_to_capacity();

  initscr();
  while(g.botbot_count() > 0 && c != 'q') {
    erase();
    g.initiate_cycle();
    printw(g.to_string().c_str());
    refresh();
    sleep(1);
  }
  endwin();

  return 0;
}
