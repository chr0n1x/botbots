#ifndef __THEGRID
#define __THEGRID

#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>
#include <iostream>

#include <pthread.h>

#include "botbot.hpp"

using namespace std;

namespace grid {

  using namespace bot_factory;

  static const int DEFAULT_GRID_DIM = 10;
  static const int MAX_BOTBOTS = 50;

  void *_botbot_creation_thread(void *);
  void *_botbot_decision_thread(void *);

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

      grid_cell() {
        bot = NULL;
        id = 0;
        row = 0;
        col = 0;
        pthread_mutex_init(&occupy_lock, NULL);
      }
      ~grid_cell() {
        if(bot != NULL) delete bot;
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
        pthread_mutex_unlock(&occupy_lock);
   
        return (bot == enterer);
      }

      /**
       *  botbot_died()
       */
      void botbot_terminated() {
        bot = NULL;
      }

      /**
       *  coordinates()
       *  Returns the coordinates in string format
       */
      string coordinates() {
        char carr[32];
        sprintf(carr, "(%d, %d)", row+1, col+1);
        const char * cc = carr;
        return string(cc);
      }

      /**
       *  get_botbot()
       */
      botbot * get_botbot() {
        return bot;
      }

      int get_row() {
        return row;
      }

      int get_col() {
        return col;
      }
  };

  class the_grid {

    /**
     *  GRID FIELDS
     */
    vector<vector <grid_cell> > grid;
    map<botbot*, grid_cell*> live_bots;
    legacy_botbot * lg;
    pthread_mutex_t population_flux_lock;
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

      pthread_mutex_init(&population_flux_lock, NULL);
    }

    /**
     *  add_botbot_to_list()
     *  Keeping record of botbots and what grid_cells they're assigned to
     *  When botbots move, the goto function will handle keeping track of
     *  all that
     */
    void add_botbot_to_list(botbot * bot, grid_cell * cell) {
      pthread_mutex_lock(&population_flux_lock);
      live_bots.insert( pair<botbot*, grid_cell*>(bot, cell) );
      pthread_mutex_unlock(&population_flux_lock);
    }

    /**
     *  cmd_decide_coordinates()
     *  Makes all botbots decide on a coordinate that they want to go to
     *  The main thread halts for this process
     */
    bool cmd_decide_coordinates() {
      int thread_ret;
      pthread_t threads[live_bots.size()];

      map<botbot*, grid_cell*>::iterator it = live_bots.begin();
      int i=0;
      for(it; it != live_bots.end(); ++it) {
        thread_ret = pthread_create( &threads[i], NULL, _botbot_decision_thread, (void*) it->first);
        if(thread_ret)
          return false;
        ++i;
      }

      // wait for threads to join back to main thread
      for(int i=0; i<live_bots.size(); ++i) {
        pthread_join(threads[i], NULL);
      }
      return true;
    }

    /**
     *  cmd_goto_coordinates()
     *  All botbots attempt to move to their desired coordinates, if stored.
     *  Otherwise, they do not move...?
     *  The main thread halts for this process
     */
    void cmd_goto_coordinates() {

      for(int i=0; i<dim; ++i) {
        for(int j=0; j<dim; ++j) {

          botbot * bot = grid[i][j].get_botbot();
          if(bot != NULL) {

            bool out_of_bounds = (bot->get_col() < 0 || bot->get_col() >= dim);
            out_of_bounds = out_of_bounds || (bot->get_row() < 0 || bot->get_row() >= dim);

            if(out_of_bounds) {
              //cout << "deleting " << bot->name() << endl;
              live_bots[bot]->botbot_terminated();
              live_bots.erase(bot);
              delete bot;
            }
            else {
              grid_cell * from_cell = live_bots[bot];
              grid_cell * to_cell = &grid[bot->get_row()][bot->get_col()];
              botbot * occupying_bot = to_cell->get_botbot();

              if(occupying_bot == NULL) {
                if(to_cell->initialize_bot(bot)) {
                  from_cell->botbot_terminated();
                  live_bots[bot] = to_cell;
                }
                else {
                  from_cell->botbot_terminated();
                  to_cell->botbot_terminated();
                  delete bot;
                  decrement_population_count();
                }
              }
              else {
                bot->set_current_cell(i, j);
              }
            }
          }

        }
      }

    }

    /**
     *  increment_population_count()
     *  It just seems right to surround this with a lock for now
     */
    void increment_population_count() {
      pthread_mutex_lock(&population_flux_lock);
      ++botizen_count;
      pthread_mutex_unlock(&population_flux_lock);
    }

    /**
     *  decrement_population_count()
     *  It just seems right to surround this with a lock for now
     */
    void decrement_population_count() {
      pthread_mutex_lock(&population_flux_lock);
      --botizen_count;
      pthread_mutex_unlock(&population_flux_lock);
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
        if(in_dim < 5) {
          in_dim = DEFAULT_GRID_DIM;
        }
        dim = in_dim;
        this->initialize();
      }
      ~the_grid() {
        delete lg;
        pthread_mutex_destroy(&population_flux_lock);
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
            b->set_current_cell(x, y);
            increment_population_count();
            add_botbot_to_list(b, &grid[x][y]);
            ret = true;
          }
          else {
            delete b;
            ret = false;
          }
        }

        return ret;
      }

      /**
       *  initiate_cycle()
       *  STEP 1: have all botbots decide where they want to go
       *  STEP 2: make them all attempt to move
       *
       *  botbots settle their own disputes
       */
      bool initiate_cycle() {
        cmd_decide_coordinates();
        cmd_goto_coordinates();
      }

      /**
       * Getters
       */
      int cell_count() {
        return dim*dim;
      }
      int botbot_count() {
        return live_bots.size();//botizen_count;
      }

      /**
       * to_string()
       *
       * Does what it says, says what it does...?
       */
      string to_string() {
        string ret = "";
        for(int i=0; i<dim; ++i) {
          for(int j=0; j<dim; ++j) {
            string bot_name = grid[i][j].get_botbot() == NULL ? "" : grid[i][j].get_botbot()->name();

            ret += "[";
            ret += bot_name;
            ret += "]";

            if(j != dim-1) 
              ret += "\t ";
          }
          ret += "\n";
        }

        return ret;
      }

      /**
       *  population_to_string()
       *  SAYS WHAT IT DOES, DEFINITELY DOES WHAT IT SAYS
       */
      string population_to_string() {
        string ret = "";
        if(live_bots.empty())
          ret = "No botbots in grid";
        else {
          map<botbot*, grid_cell*>::iterator it = live_bots.begin();
          for(it; it != live_bots.end(); ++it) {
            ret += it->second->coordinates();
            ret += "\t\t";
            ret += it->first->name();
            ret += "\n";
          }
        }

        return ret;
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

  /**
   *  _botbot_decision_thread()
   *  There has to be a better way to do this...
   */
  void *_botbot_decision_thread(void * arg) {
    botbot * bot = (botbot*) arg;
    bot->decide_movement();
    return NULL;
  }

}

#endif
