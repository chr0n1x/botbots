#ifndef __THEGRID
#define __THEGRID

#include <vector>
#include <map>

#include "internals.h"
#include "botbot.h"
#include "event.h"

using namespace bot_factory;

namespace the_grid {

static const int  DEFAULT_GRID_DIM = 5;
static const bool FULL_THREAD = false;

/**
 *  CLASS: GridCell
 *  Most basic unit of the grid
 *
 *  Implements a mutex to prevent multiple botbots from
 *  trying to occupy it (at most 2 botbots can interact over
 *  a single GridCell)
 */

class GridCell {
  int          id;
  int          row, col;
  botbot*      bot;
  mutex::Mutex occupy_lock;

  public:

    GridCell();

    ~GridCell();

    int initialize(int assigned_id, int x, int y);

    int initialize_bot(botbot* enterer);

    void botbot_terminated();

    string coordinates();

    botbot* get_botbot();

    int get_row();

    int get_col();
};

class Grid {

  typedef map<botbot*, GridCell*> botGridMap;

  // PRIVATE DATA
  bool                          battle_bots;
  vector<vector <GridCell> >    vgrid;
  map<botbot*, GridCell*>       live_bots;
  vector< pair<botbot*, int> >  dead_bots;
  legacy_botbot                *lg;
  mutex::Mutex                  population_flux_lock;
  int                           MAX_BOTBOTS;
  int                           rows, cols;
  size_t                        cycles_passed;
  vector<events::Event *>      *events;

  // PRIVATE MANIPULATORS
  void initialize();

  void add_botbot_to_list(botbot* bot, GridCell* cell);

  void move_bots();

  public:

    Grid();

    Grid(int in_rows, int in_cols, bool fish_tank_mode);

    ~Grid();

    void hold_events(vector<events::Event *> *events);

    bool fill_to_capacity();

    bool create_botizen();

    bool initiate_cycle();

    int cell_count();

    int botbot_count();

    size_t grid_cycles();

    int row_width();

    string to_string();

    string population_to_string();

    string stats_to_string();
};

}

#endif
