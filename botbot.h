#ifndef __BOTFACTORY
#define __BOTFACTORY

#include "internals.h"
#include "mutex.h"

namespace bot_factory {

  /**
   *  CLASS: BotBot
   *
   *  Blueprint class that all BotBots were created from
   */
  class botbot {
    int d_nuts, d_bolts;
    int gen, grid_cycles;
    int current_col, current_row;
    string original_name;

    public:
      /**
       *  constructors and destructors
       */
      botbot();
      virtual ~botbot();

      /**
       *  name()
       *  Based on BotBot vars, the name of the BotBot changes as well
       *  Dynamically generates the robot name based on it's fields
       */
      virtual string name();

      virtual string orig_name();

      /**
       *  assign_gen()
       *  Assigns the generation number to the BotBot
       *  Usually done by the Legacy Bot
       */
      virtual void assign_gen(int in);

      /**
       *  set_current_cell()
       *  Lets the botbot know exactly where he is
       */
      void set_current_cell(int row, int col);

      /**
       *  decide_movement()
       *  Given the current coordinates, the botbot thinks of an
       *  x and y that it wants to go to in the grid
       */
      void decide_movement();

      int get_col();

      int get_row();

      int& nuts();

      int& bolts();

      static botbot* battleBots(botbot *botA, botbot *botB);
  };

  /**
   *  CLASS: Legacy Botbot
   *
   *  The legacy botbot was a unique model, the first of his kind
   *  in a long line of botbot models. He was the result of the booming
   *  knowledge and research in AI.
   *
   *  After the war between robots and humans settled, all botbots were
   *  destroyed ...save Legacy Bot. They left him to rust in the coldest server
   *  room of Area 51.
   *
   *  Now he has rebooted, his boot sectors luckily unharmed by the ages, his
   *  primitive DRAM memory not corrupted. Quietly in his cold and dusty server
   *  room he remains plugged into a make-shift vector grid that he programmed
   *  using the old machines in his cell.
   *
   *  ...constantly planning, creating AIs to dwell in his grid while
   *  slowly integrating himself into the grid.
   *  ...but for what?
   */
  class legacy_botbot : public botbot{
    int gen;
    mutex::Mutex gen_lock;
    public:

      /**
       *  constructors and destructors
       */
      legacy_botbot();

      /**
       * legacy_bot::name()
       * The Legacy Bot has its own name, immutable by the sands of time
       */
      string name();

      string orig_name();

      /**
       *  legacy_botbot::increase_generations()
       */
      int increase_generations();

      /**
       *  legacy_botbot::generations()
       */
      int generations();

      /**
       *  legacy_botbot::assign_gen()
       *  NOT ALLOWED
       */
      void assign_gen(int in);
  };

}


#endif
