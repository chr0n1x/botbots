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

// BATTLEBOTS
const int MAX_MULTIPLIER   = 4;
const int MIN_NUTS_TO_LOSE = 5;
const int SURRENDER_MOD    = 3;
botbot* botbot::battleBots(botbot *botA, botbot *botB)
{
    // roll dice for random multipliers!
    int nutMultA  = rand() % MAX_MULTIPLIER;
    int nutMultB  = rand() % MAX_MULTIPLIER;
    int boltMultA = rand() % MAX_MULTIPLIER;
    int boltMultB = rand() % MAX_MULTIPLIER;

    int botAScore = botA->nuts() * nutMultA + botA->bolts() * boltMultA;
    int botBScore = botB->nuts() * nutMultB + botB->bolts() * boltMultB;

    botbot *winner  = (botAScore < botBScore) ? botB : botA;
    int winnerScore = max(botAScore, botBScore);
    botbot *loser   = (botAScore > botBScore) ? botB : botA;
    int loserScore  = max(botBScore, botAScore);

    // We must punish the loser and reward the winner

    // But first...if the loser is far overpowered, we give him a chance to
    // escape
    if (loserScore < winnerScore/2 && (rand() % SURRENDER_MOD) == 0) {
        return winner;
    }

    // Transfer some nuts and bolts to the winner
    int numNutsToLose  = (loser->nuts() < MIN_NUTS_TO_LOSE)
                       ? loser->nuts() : rand() % (loser->nuts() / 2);
    int numBoltsToLose = (loser->bolts() < MIN_NUTS_TO_LOSE)
                       ? loser->bolts() : rand() % (loser->bolts() / 2);

    loser->nuts()   -= numNutsToLose;
    winner->nuts()  += numNutsToLose;
    loser->bolts()  -= numBoltsToLose;
    winner->bolts() += numBoltsToLose;

    return winner;
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
