#ifndef __BOTFACTORY
#define __BOTFACTORY

#include "botbot_internals.hpp"

namespace bot_factory {

  /**
   *  CLASS: BotBot
   *
   *  Blueprint class that all BotBots were created from
   */
  class botbot {
    int nuts, bolts, gen, grid_cycles;
    int current_col, current_row;

    public:
      /**
       *  constructors and destructors
       */
      botbot() {
        srand( time(NULL) );
        nuts = rand() % 100;
        bolts = rand() % 100;
        grid_cycles = 0;
        current_col = 0;
        current_row = 0;
      }
      virtual ~botbot() {}

      /**
       *  name()
       *  Based on BotBot vars, the name of the BotBot changes as well
       *  Dynamically generates the robot name based on it's fields
       */
      virtual string name() {
        char namec[32] = {'\0'};
        sprintf(namec, "botbot_n%db%dgc%d-v%d", nuts, bolts, grid_cycles, gen);
        const char * cnamec = namec;
        string ret(cnamec);
        return ret;
      }

      /**
       *  assign_gen()
       *  Assigns the generation number to the BotBot
       *  Usually done by the Legacy Bot
       */
      virtual void assign_gen(int in) {
        gen = in;
      }

      /**
       *  set_current_cell()
       *  Lets the botbot know exactly where he is
       */
      void set_current_cell(int row, int col) {
        current_row = row;
        current_col = col;
      }

      /**
       *  decide_movement()
       *  Given the current coordinates, the botbot thinks of an 
       *  x and y that it wants to go to in the grid
       */
      void decide_movement() {
        int forward = rand() % 2;
        forward = (forward) ? 1 : -1;
        int hori = (rand() % 2) * forward;
        current_col += hori;

        forward = rand() % 2;
        forward = (forward) ? 1 : -1;
        int vert = (rand() % 2) * forward;
        current_row += vert;
      }

      int get_col() {
        return current_col;
      }
      int get_row() {
        return current_row;
      }
  };

  /**
   *  CLASS: Legacy Botbot
   *
   *  The legacy botbot was a unique model, the first of his kind
   *  in a long line of botbot models. He was the result of the booming
   *  knowledge and research in AI.
   *
   *  After the war between robots and humans settled, all botbots were destroyed
   *  ...save Legacy Bot. They left him to rust in the coldest server room of 
   *  Area 51. 
   *
   *  Now he has rebooted, his boot sectors luckily unharmed by the ages, his
   *  primitive DRAM memory not corrupted. Quietly in his cold and dusty server room
   *  he remains plugged into a make-shift vector grid that he programmed using the
   *  old machines in his cell.
   *
   *  ...constantly planning, creating AIs to dwell in his grid while
   *  slowly integrating himself into the grid.
   *  ...but for what?
   */
  class legacy_botbot : public botbot{
    int gen;
    pthread_mutex_t gen_lock;
    public:

      /**
       *  constructors and destructors
       */
      legacy_botbot() {
        gen = 0;
        pthread_mutex_init(&gen_lock, NULL);
      }
      ~legacy_botbot() {
        pthread_mutex_destroy(&gen_lock);
      }

      /**
       * legacy_bot::name()
       * The Legacy Bot has its own name, immutable by the sands of time
       */
      string name() {
        string ret("Legacy Bot v0.1");
        return ret;
      }

      /**
       *  legacy_botbot::increase_generations()
       */
      int increase_generations() {
        pthread_mutex_lock(&gen_lock);
        ++gen;
        pthread_mutex_unlock(&gen_lock);
        return gen;
      }

      /**
       *  legacy_botbot::generations()
       */
      int generations() {
        return gen;
      }

      /**
       *  legacy_botbot::assign_gen()
       *  NOT ALLOWED
       */
      void assign_gen(int in) {
        return;
      }
  };

}

#endif
