#ifndef __THEGRID
#define __THEGRID

#include <vector>
#include <map>

#include "internals.h"
#include "botbot.h"


using namespace bot_factory;


namespace the_grid {

  static const int  DEFAULT_GRID_DIM = 5;
  static const bool FULL_THREAD = false;

  void *_botbot_creation_thread(void*);
  void *_botbot_decision_thread(void*);

  /**
   *  CLASS: grid_cell
   *  Most basic unit of the grid
   *
   *  Implements a mutex to prevent multiple botbots from
   *  trying to occupy it (at most 2 botbots can interact over
   *  a single grid_cell)
   */
  class grid_cell {
    int          id;
    int          row, col;
    botbot*      bot;
    mutex::Mutex occupy_lock;

    public:

      grid_cell();

      ~grid_cell();

      int initialize(int assigned_id, int x, int y);

      int initialize_bot(botbot* enterer);

      void botbot_terminated();

      string coordinates();

      botbot* get_botbot();

      int get_row();

      int get_col();
  };

  class grid {

    vector<vector <grid_cell> > vgrid;
    map<botbot*, grid_cell*>    live_bots;
    map<botbot*, int>           dead_bots;

    legacy_botbot* lg;

    mutex::Mutex population_flux_lock;

    int MAX_BOTBOTS;
    int rows, cols;
    size_t cycles_passed;

    void initialize();

    void add_botbot_to_list(botbot* bot, grid_cell* cell);

    bool cmd_decide_coordinates();

    void cmd_goto_coordinates();

    public:

      grid();

      grid(int in_rows, int in_cols);

      ~grid();

      bool fill_to_capacity();

      bool create_botizen();

      bool initiate_cycle();

      int cell_count();

      int botbot_count();

      size_t grid_cycles();

      int row_width();

      string to_string();

      string population_to_string();
  };

}

#endif
