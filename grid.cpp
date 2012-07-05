#include "grid.h"
#include <algorithm>

using namespace the_grid;

//---------------------------------------------------
// grid_cell CLASS
//---------------------------------------------------

grid_cell::grid_cell() {
  bot = NULL;
  id = 0;
  row = 0;
  col = 0;
}

grid_cell::~grid_cell() {
    delete bot;
}

/**
 *  initialize()
 *  (int) assigned_id   Arbitrary integer assigned to the cell
 *  (int) x             Row
 *  (int) y             Column
 *
 *  Just initializes the cell with an id and the coordinates
 */
int grid_cell::initialize(int assigned_id, int x, int y) {
  if(bot == NULL) {
    id = assigned_id;
    row = x;
    col = y;
  }
}

/**
 *  initialize_bot()
 *  (botbot*) enterer   The botbot that wants to go into this cell
 *
 *  given a botbot, locks the cell and attempts to place
 *  the botbot into it
 */
int grid_cell::initialize_bot(botbot* enterer)
{
    if(bot == NULL) {
        bot = enterer;
    }
    return (bot == enterer);
}

/**
 *  botbot_terminated()
 *
 *  Called if the botbot moves or dies
 */
void grid_cell::botbot_terminated() {
    bot = NULL;
}

/**
 *  coordinates()
 *
 *  Returns the coordinates in string format
 */
string grid_cell::coordinates() {
  char carr[32];
  sprintf(carr, "(%d, %d)", row+1, col+1);
  const char * cc = carr;
  return string(cc);
}

/**
 * Getters
 */
botbot* grid_cell::get_botbot() {
  return bot;
}

int grid_cell::get_row() {
  return row;
}

int grid_cell::get_col() {
  return col;
}


//---------------------------------------------------
// grid CLASS
//---------------------------------------------------

//---------------------------------------------------
// PRIVATE FUNCTIONS
//---------------------------------------------------

/**
 *  initialize()
 *
 *  function that just builds the grid...moved to a separate function
 *  to keep the constructors DRY
 */
void grid::initialize() {
  lg = new legacy_botbot();
  cycles_passed = 0;

  MAX_BOTBOTS = rows*cols / 5;

  vgrid.resize(rows);

  grid_cell base_cell;
  int cell_count = 0;
  for(int i=0; i<rows; ++i)
    {
        for(int j=0; j<cols; ++j)
        {
            vgrid[i].push_back(base_cell);
            vgrid[i][j].initialize(cell_count, i, j);
            ++cell_count;
        }
    }
}

/**
 *  add_botbot_to_list()
 *  (botbot*)     bot         The botbot and...
 *  (grid_cell*)  cell        His cell
 *
 *  Keeping record of botbots and what grid_cells they're assigned to
 *  When botbots move, the goto function will handle keeping track of
 *  all that
 */
void grid::add_botbot_to_list(botbot* bot, grid_cell* cell) {
  population_flux_lock.lock();
  live_bots.insert( pair<botbot*, grid_cell*>(bot, cell) );
  population_flux_lock.unlock();
}

/**
 *  cmd_decide_coordinates()
 *
 *  Makes all botbots decide on a coordinate that they want to go to
 */
bool grid::cmd_decide_coordinates() {
  map<botbot*, grid_cell*>::iterator it = live_bots.begin();
  for(it; it != live_bots.end(); ++it) {
    it->first->decide_movement();
  }
  return true;
}

/**
 *  cmd_goto_coordinates()
 *
 *  cmd_decide_coordinates() should be called before this
 *  so that the botbots have desired coordinates to go to
 *
 *  All botbots attempt to move to their desired coordinates
 *  Otherwise, they do not move...?
 *  The main thread halts for this process
 */
void grid::cmd_goto_coordinates() {
  for(int i=0; i<rows; ++i)
  {
      for(int j=0; j<cols; ++j)
      {
          botbot * bot = vgrid[i][j].get_botbot();

          if (bot == NULL) continue;

          // case where the bot tries to go out of the grid
          // dirty traitors...
          bool out_of_bounds = (bot->get_col() < 0
                             || bot->get_col() >= cols)
                             || (bot->get_row() < 0
                             || bot->get_row() >= rows);

          if(out_of_bounds) {
              bot->set_current_cell(i, j);
              continue;
          }

          grid_cell* from_cell = live_bots[bot];
          grid_cell* to_cell = &vgrid[bot->get_row()][bot->get_col()];
          botbot* occupying_bot = to_cell->get_botbot();

          if(occupying_bot == NULL) {
              // attempt to initialize cell with botbot
              if(to_cell->initialize_bot(bot)) {
                  from_cell->botbot_terminated();
                  live_bots[bot] = to_cell;
              }
              else {
                  // bad luck, mate
                  from_cell->botbot_terminated();
                  to_cell->botbot_terminated();
                  live_bots.erase(bot);
                  dead_bots[bot] = cycles_passed;
              }
          }

          // there is another botbot here --
          // FIGHT
          else if (battle_bots) {
              botbot* winner = botbot::battleBots(occupying_bot, bot);

              if (winner == bot) {
                  from_cell->botbot_terminated();
                  to_cell->botbot_terminated();
                  to_cell->initialize_bot(bot);
                  live_bots[bot] = to_cell;

                  if (occupying_bot->nuts()  == 0
                   && occupying_bot->bolts() == 0)
                  {
                      live_bots.erase(occupying_bot);
                      dead_bots[occupying_bot] = cycles_passed;
                  }
                  else {
                      from_cell->initialize_bot(occupying_bot);
                      occupying_bot->set_current_cell(i, j);
                      live_bots[occupying_bot] = from_cell;
                  }
              } else if (bot->nuts() == 0 && bot->bolts() == 0) {
                  from_cell->botbot_terminated();
                  live_bots.erase(bot);
                  dead_bots[bot] = cycles_passed;
              }
              else {
                  bot->set_current_cell(i, j);
              }
          }
      }
  }
}


//---------------------------------------------------
// PUBLIC FUNCTIONS
//---------------------------------------------------

/**
 *  default and explicit constructors, destructor
 */
grid::grid() {
  rows = DEFAULT_GRID_DIM;
  cols = DEFAULT_GRID_DIM;
  battle_bots = false;
  this->initialize();
}

grid::grid(int in_rows, int in_cols, bool fish_tank_mode) {
  if(in_rows < DEFAULT_GRID_DIM) {
      in_rows = DEFAULT_GRID_DIM;
  }
  rows = in_rows;

  if(in_cols < DEFAULT_GRID_DIM) {
      in_cols = DEFAULT_GRID_DIM;
  }
  cols = in_cols;

  battle_bots = !fish_tank_mode;

  this->initialize();
}

grid::~grid() {
  delete lg;
}

/**
 *  fill_to_capacity()
 *
 *  Inserts botbots into the grid until it's to capacity (based on
 *  MAX_BOTBOTS) Returns a boolean based on success or failure
 */
bool grid::fill_to_capacity() {
    if(live_bots.size() >= MAX_BOTBOTS) {
        return false;
    }

    // create vector of open cells
    vector<grid_cell*> empty_cells;
    for(int i = 0; i < vgrid.size(); ++i)
    {
        vector<grid_cell>& currRow = vgrid[i];
        for(int j = 0; j < currRow.size(); ++j)
        {
            if (currRow[j].get_botbot() == NULL) {
                empty_cells.push_back(&currRow[j]);
            }
        }
    }

    // mix that shit up
    random_shuffle(empty_cells.begin(), empty_cells.end());

    int numNewBots = (MAX_BOTBOTS < empty_cells.size()) ? MAX_BOTBOTS
                                                        : empty_cells.size();

    for(int i = 0; i < numNewBots; ++i)
    {
        grid_cell* cell = empty_cells[i];
        botbot* b = new botbot();
        b->assign_gen(lg->increase_generations());
        cell->initialize_bot(b);
        b->set_current_cell(cell->get_row(), cell->get_col());
        add_botbot_to_list(b, cell);
    }
    return true;
}

/**
 *  initiate_cycle()
 *  STEP 0: increment the number of cycles
 *  STEP 1: have all botbots decide where they want to go
 *  STEP 2: make them all attempt to move
 *
 *  botbots settle their own disputes
 *
 *  TODO: Make this wake when tasks are complete, instead of sleeping for a flat second
 *        Also, maybe have the grid calculate the average cycle duration...?
 */
bool grid::initiate_cycle() {
  if (live_bots.size() > 1) {
      ++cycles_passed;
      cmd_decide_coordinates();
      cmd_goto_coordinates();
  }
  sleep(1);
}

/**
 * Getters
 */
int grid::cell_count() {
  return rows*cols;
}

int grid::botbot_count() {
  return live_bots.size();
}

size_t grid::grid_cycles() {
  return cycles_passed;
}

/**
 *  row_width()
 *
 *  Based on any name of any botbot, returns the length that a line
 *  should be based on the length of the name and the number of columns
 */
int grid::row_width() {
  int ret = 0;
  if(live_bots.size() > 0) {
      ret = ((live_bots.begin()->first->name()).length()) / cols;
  }
  return ret;
}

/**
 * to_string()
 *
 * Does what it says, says what it does...?
 */
string grid::to_string() {
  if(live_bots.size() > 0) {
    int line_width = row_width();

    // get max bot name length
    int maxNameLength = 0;
    for(map<botbot*, grid_cell*>::iterator it = live_bots.begin();
        it != live_bots.end();
        ++it)
    {
        if (it->first->name().size() > maxNameLength) {
            maxNameLength = it->first->name().size();
        }
    }
    string white_space_filler(maxNameLength, ' ');

    // TODO: think of a way to encapsulate this in a class or classes
    stringstream buffer;
    streambuf * def = cout.rdbuf(buffer.rdbuf());

    for(int i=0; i<rows; ++i) {

      for(int j=0; j<cols; ++j) {
        botbot* bot = vgrid[i][j].get_botbot();
        string bot_name =
            (bot == NULL || bot == 0) ? white_space_filler :
                                        vgrid[i][j].get_botbot()->name();

        if(bot_name.length() !=  white_space_filler.length()) {
            int diff  = white_space_filler.length() - bot_name.length();
            bot_name += white_space_filler.substr(0, diff);
        }
        cout << setw(line_width);
        cout << '[' << bot_name << ']';
      }
      cout << endl << endl;
    }
    cout << endl << "----- " << live_bots.size()
         << " BotBots Online (" << cycles_passed << " Cycles) -----";
    cout << endl << endl << population_to_string() << endl;

    if(!battle_bots) {
      cout << endl << "FISH TANK MODE" << endl;
    }

    string ret = buffer.str();
    cout.rdbuf(def);
    return ret;
  }
  return "ALL BOTBOTS DIED\n";
}

/**
 *  population_to_string()
 *
 *  SAYS WHAT IT DOES, DEFINITELY DOES WHAT IT SAYS
 */
string grid::population_to_string() {
  int line_width = row_width();

  // TODO: think of a way to encapsulate this in a class or classes
  stringstream buffer;
  streambuf * def = cout.rdbuf(buffer.rdbuf());
  cout << setfill(' ') << setw(line_width);

  if(live_bots.empty())
    cout << "No botbots in grid" << endl;
  else {
    map<botbot*, grid_cell*>::iterator it = live_bots.begin();
    int col = 1;
    int numCols = cols/3;
    cout << "Living botbots: " << endl;
    for(it; it != live_bots.end(); ++it, ++col)
    {
        cout << it->second->coordinates();
        cout << (it->second->coordinates().length() > 7 ? "\t" : "\t\t");
        cout << it->first->name();
        cout << ((col % numCols == 0) ? "\n" : "\t|\t");
    }

    if (!dead_bots.empty()) {
        cout << endl << endl << "Fallen Bots: " << endl;
        col = 1;
        map<botbot*, int>::iterator it;
        for (it = dead_bots.begin(); it != dead_bots.end(); ++it, ++col)
        {
            cout << it->second << "\t\t"
                 << it->first->orig_name();
            cout << ((col % numCols == 0) ? "\n" : "\t|\t");
        }
    }
    if (live_bots.size() == 1) {
        cout << "Winner is " << live_bots.begin()->first->orig_name()
             << "!!" << endl;
    }
  }

  string ret = buffer.str();
  cout.rdbuf(def);
  return ret;
}
