#ifndef __THEGRID
#define __THEGRID

#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <vector>
#include <iostream>
#include "botbot.hpp"

using namespace std;

namespace grid {

  using namespace bot_factory;

  static const int DEFAULT_GRID_DIM = 10;
  static const int MAX_BOTBOTS = 10;

  class the_grid {

    /**
     *  CLASS: grid_cell
     *  Most basic unit of the grid
     */
    class grid_cell {
      int id;
      int row, col;
      botbot * bot;
      pthread_mutex_t occupy_lock;
 
      public:

        ~grid_cell() {
          if(bot) delete bot;
          pthread_mutex_destroy(&occupy_lock);
        }

        int initialize(int assigned_id, int x, int y) {
          if(bot == NULL) {
            id = assigned_id;
            row = x;
            col = y;
            return pthread_mutex_init(&occupy_lock, NULL);
          }
        }

        int initialize_bot(botbot * enterer) {

          pthread_mutex_lock(&occupy_lock);

          if(bot == NULL) {
            bot = enterer;
          }
          else {
            cout << "grid::grid_cell_" << id << "::initialize() -- FAIL" << endl;
            cout << "\tOccupier::Bot_" << bot->name() << endl;
          }

          pthread_mutex_unlock(&occupy_lock);
     
          return (bot == enterer);
        }

        botbot * get_botbot() {
          return bot;
        }
    };

    /**
     *  GRID FIELDS
     */
    vector<vector <grid_cell> > grid;
    legacy_botbot * lg;
    pthread_mutex_t botbot_creation_lock;
    int dim, botizen_count;

    /**
     *  PRIVATE FUNCTIONS
     */
    void initialize() {
      lg = new legacy_botbot();
      cout << "Created " << lg->name() << endl;
      lg->assign_gen(2);

      botizen_count = 0;
      cout << "INITIALIZING GRID WITH DIM " << dim << endl;

      vector<grid_cell> row;
      for(int i=0; i<dim; ++i) {
        grid.push_back(row);
      }

      grid_cell base_cell;
      for(int i=0; i<dim; ++i) {
        for(int j=0; j<dim; ++j) {
          grid[i].push_back(base_cell);
          grid[i][j].initialize(i*j, i, j);
        }
      }

      pthread_mutex_init(&botbot_creation_lock, NULL);
    }

    public:
      the_grid() {
        dim = DEFAULT_GRID_DIM;
        this->initialize();
      }
      the_grid(int in_dim) {
        if(in_dim < 5 || in_dim > 20) {
          cout << "DIMENSIONS TOO LARGE FOR GRID, DEFAULTING TO " << DEFAULT_GRID_DIM << endl;
          in_dim = DEFAULT_GRID_DIM;
        }
        dim = in_dim;
        this->initialize();
      }
      ~the_grid() {
        delete lg;
      }

      bool create_botizen() {

        pthread_mutex_lock(&botbot_creation_lock);

        bool ret;
        if(botizen_count == dim*dim || botizen_count == MAX_BOTBOTS) {
          cout << "Cannot create a new botbot::full capacity" << endl;
          ret = false;
        }
        else {
          botbot * b = new botbot();
          b->assign_gen(lg->increase_generations());
          int x, y;
          do {
            srand( time(NULL) );
            x = rand() % dim;
            y = rand() % dim;
          }
          while(grid[x][y].get_botbot() != NULL);
          grid[x][y].initialize_bot(b);
          ++botizen_count;
          ret = true;
        }

        pthread_mutex_unlock(&botbot_creation_lock);

        return ret;
      }

      int cell_count() {
        return dim*dim;
      }

      void print_to_console() {
        for(int i=0; i<dim; ++i) {
          for(int j=0; j<dim; ++j) {
            string bot_name = grid[i][j].get_botbot() == NULL ? "No botbot" : grid[i][j].get_botbot()->name();
            cout << "(" << i << "," << j << "): " << bot_name << "\t\t\t\t";
          }
          cout << endl << endl;
        }
      }
  };

}

#endif
