#include "grid.hpp"

#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char ** argv) {

  int grid_dim = 10;
  if(argc > 1) {
    grid_dim = atoi(argv[1]);
  }
  grid::the_grid g(grid_dim);
  cout << "Grid cells: " << g.cell_count() << endl;

  cout << "Populating grid with botbots..." << endl << endl;
  g.fill_to_capacity();

  cout << g.to_string() << endl << endl;
  cout << g.botbot_count() << " botbots created" << endl;
  cout << g.population_to_string() << endl;
  return 0;
}
