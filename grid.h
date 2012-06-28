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

      grid_cell() {
        bot = NULL;
        id = 0;
        row = 0;
        col = 0;
      }

      ~grid_cell() {
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
      int initialize(int assigned_id, int x, int y) {
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
      int initialize_bot(botbot* enterer)
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
      void botbot_terminated() {
          bot = NULL;
      }

      /**
       *  coordinates()
       *
       *  Returns the coordinates in string format
       */
      string coordinates() {
        char carr[32];
        sprintf(carr, "(%d, %d)", row+1, col+1);
        const char * cc = carr;
        return string(cc);
      }

      /**
       * Getters
       */
      botbot* get_botbot() {
        return bot;
      }

      int get_row() {
        return row;
      }

      int get_col() {
        return col;
      }
  };

  class grid {

    /**
     *  GRID FIELDS
     */
    vector<vector <grid_cell> > vgrid;
    map<botbot*, grid_cell*>    live_bots;
    map<botbot*, int>           dead_bots;

    legacy_botbot* lg;

    mutex::Mutex population_flux_lock;

    int MAX_BOTBOTS;
    int rows, cols;
    size_t cycles_passed;

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
    void add_botbot_to_list(botbot* bot, grid_cell* cell) {
      population_flux_lock.lock();
      live_bots.insert( pair<botbot*, grid_cell*>(bot, cell) );
      population_flux_lock.unlock();
    }

    /**
     *  cmd_decide_coordinates()
     *
     *  Makes all botbots decide on a coordinate that they want to go to
     */
    bool cmd_decide_coordinates()
    {
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
    void cmd_goto_coordinates() {
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
                else {
                    botbot* winner = battleBots(occupying_bot, bot);

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

    public:

      /**
       *  default and explicit constructors, destructor
       */
      grid() {
          rows = DEFAULT_GRID_DIM;
          cols = DEFAULT_GRID_DIM;
          this->initialize();
      }

      grid(int in_rows, int in_cols) {
          if(in_rows < DEFAULT_GRID_DIM) {
              in_rows = DEFAULT_GRID_DIM;
          }
          rows = in_rows;

          if(in_cols < DEFAULT_GRID_DIM) {
              in_cols = DEFAULT_GRID_DIM;
          }
          cols = in_cols;

          this->initialize();
      }

      ~grid() {
          delete lg;
      }

      /**
       *  fill_to_capacity()
       *
       *  Inserts botbots into the grid until it's to capacity (based on
       *  MAX_BOTBOTS) Returns a boolean based on success or failure
       */
      bool fill_to_capacity() {
          if(live_bots.size() >= MAX_BOTBOTS) {
              return false;
          }
          for(int i=0; i<MAX_BOTBOTS; ++i) {
              create_botizen();
          }
          return true;
      }


      /**
       *  create_botizen()
       *
       *  Makes a botbot and throws it onto the grid
       */
      bool create_botizen()
      {
          bool ret;
          if (live_bots.size() == rows*cols
           || live_bots.size() >= MAX_BOTBOTS)
          {
              ret = false;
          }
          else {
              botbot* b = new botbot();
              b->assign_gen(lg->increase_generations());
              int x, y;
              do {
                  x = rand() % rows;
                  y = rand() % cols;
              }
              while(vgrid[x][y].get_botbot() != NULL);

              if(vgrid[x][y].initialize_bot(b)) {
                  b->set_current_cell(x, y);
                  add_botbot_to_list(b, &vgrid[x][y]);
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
       *  STEP 0: increment the number of cycles
       *  STEP 1: have all botbots decide where they want to go
       *  STEP 2: make them all attempt to move
       *
       *  botbots settle their own disputes
       */
      bool initiate_cycle() {
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
      int cell_count() {
          return rows*cols;
      }

      int botbot_count() {
          return live_bots.size();
      }

      size_t grid_cycles() {
          return cycles_passed;
      }

      /**
       *  row_width()
       *
       *  Based on any name of any botbot, returns the length that a line
       *  should be based on the length of the name and the number of columns
       */
      int row_width() {
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
      string to_string() {
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
          //cout << setfill(' ') << setw(line_width);

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
      string population_to_string() {
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
  };

}

#endif
