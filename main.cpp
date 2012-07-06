#include "grid.h"

#include <iostream>
#include <cstdlib>

#include <ncurses.h>

using namespace std;

int main(int argc, char ** argv)
{
  srand( time(NULL) );

  int grid_rows = 0;
  int grid_cols = 0;
  bool fish_tank = false;

  if(argc >= 3) {
    grid_rows = atoi(argv[1]);
    grid_cols = atoi(argv[2]);
    if (argc == 4) {
      string option(argv[3]);

      int dash_pos = option.find("-", 0);
      while(dash_pos != string::npos) {
        option.erase(dash_pos, 1);
        dash_pos = option.find("-", 0);
      }

      fish_tank = option.compare("fishtank") == 0 || option.compare("f") == 0;

      if(!fish_tank) {
        cout << "Unrecognized option: " << option << " ...stopping" << endl;
        return 0;
      }
    }
  }
  the_grid::grid g(grid_rows, grid_cols, fish_tank);
  cout << "Initializing Grid..." << endl;
  g.fill_to_capacity();

  initscr();
  while(g.botbot_count() > 0)
  {
    erase();
    g.initiate_cycle();
    printw(g.to_string().c_str());
    refresh();
  }
  endwin();

  return 0;
}
