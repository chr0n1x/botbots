#include "botbot.h"
using namespace bot_factory;

/**
 *  CLASS: BotBot
 */
botbot::botbot()
{
    d_nuts = rand() % 100;
    d_bolts = rand() % 100;
    grid_cycles = 0;
    current_col = 0;
    current_row = 0;
    original_name = name();
}
botbot::~botbot() {}

/**
 *  name()
 *  Based on BotBot vars, the name of the BotBot changes as well
 *  Dynamically generates the robot name based on it's fields
 */
string botbot::name() {
  char namec[32] = {'\0'};
  sprintf(namec, "bb_n%02db%02d-v%d",
          d_nuts, d_bolts, gen);
  return namec;
}

string botbot::orig_name() {
    return original_name;
}

/**
 *  assign_gen()
 *  Assigns the generation number to the BotBot
 *  Usually done by the Legacy Bot
 */
void botbot::assign_gen(int in) {
    gen = in;
    original_name = name();
}

/**
 *  set_current_cell()
 *  Lets the botbot know exactly where he is
 */
void botbot::set_current_cell(int row, int col) {
  current_row = row;
  current_col = col;
}

/**
 *  decide_movement()
 *  Given the current coordinates, the botbot thinks of an
 *  x and y that it wants to go to in the grid
 */
void botbot::decide_movement() {
  current_col += rand() % 3 - 1;
  current_row += rand() % 3 - 1;
}

int botbot::get_col() {
  return current_col;
}

int botbot::get_row() {
  return current_row;
}

int& botbot::nuts() {
    return d_nuts;
}

int& botbot::bolts() {
    return d_bolts;
}

/**
 *  CLASS: Legacy Botbot
 */

/**
 *  constructors and destructors
 */
legacy_botbot::legacy_botbot() {
  gen = 0;
}

/**
 * legacy_bot::name()
 * The Legacy Bot has its own name, immutable by the sands of time
 */
string legacy_botbot::name() {
  return "Legacy Bot v0.1";
}

string legacy_botbot::orig_name() {
    return name();
}

/**
 *  legacy_botbot::increase_generations()
 */
int legacy_botbot::increase_generations() {
  gen_lock.lock();
  ++gen;
  gen_lock.unlock();
  return gen;
}

/**
 *  legacy_botbot::generations()
 */
int legacy_botbot::generations() {
  return gen;
}

/**
 *  legacy_botbot::assign_gen()
 *  NOT ALLOWED
 */
void legacy_botbot::assign_gen(int in) {
  return;
}
