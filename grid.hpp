#ifndef __THEGRID
#define __THEGRID

#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

#include <pthread.h>

#include "botbot.hpp"

using namespace std;

namespace grid {

  using namespace bot_factory;

  static const int DEFAULT_GRID_DIM = 10;
  static const int MAX_BOTBOTS = 100;

  void *_botbot_creation_thread(void *);

  class the_grid {

    /**
     *  CLASS: grid_cell
     *  Most basic unit of the grid
     *
     *  Implements a mutex to prevent multiple botbots from
     *  trying to occupy it (at most 2 botbots can interact over
     *  a single grid_cell)
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

        /**
         *  initialize()
         *  Just initializes the cell with an id and the coordinates
         */
        int initialize(int assigned_id, int x, int y) {
          if(bot == NULL) {
            id = assigned_id;
            row = x;
            col = y;
            return pthread_mutex_init(&occupy_lock, NULL);
          }
        }

        /**
         *  initialize_bot()
         *
         *  given a botbot, locks the cell and attempts to place
         *  the botbot into it
         */
        int initialize_bot(botbot * enterer) {

          pthread_mutex_lock(&occupy_lock);

          if(bot == NULL) {
            bot = enterer;
          }
          /*
          else {
            cout << "grid::grid_cell_" << id << "::initialize() -- FAIL" << endl;
            cout << "\tOccupier::Bot_" << bot->name() << endl;
          }
          */

          pthread_mutex_unlock(&occupy_lock);
     
          return (bot == enterer);
        }

        /**
         *  get_botbot()
         */
        botbot * get_botbot() {
          return bot;
        }
    };

    /**
     *  GRID FIELDS
     */
    vector<vector <grid_cell> > grid;
    legacy_botbot * lg;
    pthread_mutex_t botbot_census_count_lock;
    int dim, botizen_count;

    /**
     *  PRIVATE FUNCTIONS
     */

    /**
     *  initialize()
     *
     *  function that just builds the grid...moved to a separate function
     *  to keep the constructors DRY
     */
    void initialize() {
      lg = new legacy_botbot();
      botizen_count = 0;

      vector<grid_cell> row;
      for(int i=0; i<dim; ++i) {
        grid.push_back(row);
      }

      grid_cell base_cell;
      int cell_count = 0;
      for(int i=0; i<dim; ++i) {
        for(int j=0; j<dim; ++j) {
          grid[i].push_back(base_cell);
          grid[i][j].initialize(cell_count, i, j);
          ++cell_count;
        }
      }

      pthread_mutex_init(&botbot_census_count_lock, NULL);
    }

    /**
     *  increment_population_count()
     *  It just seems right to surround this with a lock for now
     */
    void increment_population_count() {
      pthread_mutex_lock(&botbot_census_count_lock);
      ++botizen_count;
      pthread_mutex_unlock(&botbot_census_count_lock);
    }

    /**
     *  decrement_population_count()
     *  It just seems right to surround this with a lock for now
     */
    void decrement_population_count() {
      pthread_mutex_lock(&botbot_census_count_lock);
      --botizen_count;
      pthread_mutex_unlock(&botbot_census_count_lock);
    }

    public:

      /**
       *  default and explicit constructors, destructor
       */
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
        pthread_mutex_destroy(&botbot_census_count_lock);
      }

      /**
       *  fill_to_capacity()
       *  Inserts botbots into the grid until it's to capacity (based on MAX_BOTBOTS)
       *  Returns a boolean based on success or failure
       */
      bool fill_to_capacity() {
        if(botizen_count >= MAX_BOTBOTS)
          return false;
        else {
          pthread_mutex_t ready_lock;
          if(pthread_mutex_init(&ready_lock, NULL) != 0)
            return false;

          // create threads
          int thread_ret;
          pthread_t threads[MAX_BOTBOTS];
          for(int i=0; i<MAX_BOTBOTS; ++i) {
            thread_ret = pthread_create( &threads[i], NULL, _botbot_creation_thread, (void*)this );
            if(thread_ret)
              return false;
          }

          // wait for threads to join back to main thread
          for(int i=0; i<MAX_BOTBOTS; ++i) {
            pthread_join(threads[i], NULL);
          }

          return true;
        }
      }

      /**
       *  create_botizen()
       *  Makes a botbot and throws it onto the grid
       */
      bool create_botizen() {

        bool ret;
        if(botizen_count == dim*dim || botizen_count == MAX_BOTBOTS) {
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

          if(grid[x][y].initialize_bot(b)) {
            //cout << "Initialized " << b->name() << endl;
            increment_population_count();
            ret = true;
          }
          else {
            //cout << "FAILED " << b->name() << endl;
            delete b;
            //decrement_population_count();
            ret = false;
          }
        }

        return ret;
      }

      /**
       * Getters
       */
      int cell_count() {
        return dim*dim;
      }
      int botbot_count() {
        return botizen_count;
      }

      /**
       *  print_to_console()
       *  Does what it says, says what it does
       */
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

  /**
   *  _botbot_creation_thread()
   *  function intended to be used as a thread only!
   *  used in fill_to_capacity()
   */
  void *_botbot_creation_thread(void * arg) {
    the_grid * grid = (the_grid*) arg;
    bool res = grid->create_botizen();
    return NULL;
  }

}

#endif
