#include "event.h"
#include <iostream>
#include <sstream>

using namespace std;
namespace {

const char happyBotBots[]    = "<insert happy robot ascii art here>";
const char fightingBotBots[] = "|@|@|@|@|           |@|@|@|@|\n"
                               "|@|@|@|@|   _____   |@|@|@|@|\n"
                               "|@|@|@|@| /\\_T_T_/\\ |@|@|@|@|\n"
                               "|@|@|@|@||/\\ T T /\\||@|@|@|@|\n"
                               " ~/T~~T~||~\\/~T~\\/~||~T~~T\\~ \n"
                               "  \\|__|_| \\(-(O)-)/ |_|__|/  \n"
                               "  _| _|    \\\\8_8//    |_ |_  \n"
                               "|(@)]   /~~[_____]~~\\   [(@)| TODO - get/make"
                               " better ascii art\n"
                               "  ~    (  |       |  )    ~  \n"
                               "      [~` ]       [ '~]      \n"
                               "      |~~|         |~~|      \n"
                               "      |  |         |  |      \n"
                               "     _<\\/>_       _<\\/>_     \n"
                               "    /_====_\\     /_====_\\    \n";

} // close unnamed namespace

namespace events {

                            //============
                            // class Event
                            //============

Event::~Event()
{
}

                            //==================
                            // class BattleEvent
                            //==================

BattleEvent::BattleEvent(int numNuts,
                         int numBolts,
                         const string& winner,
                         const string& loser)
: d_nuts_transferred(numNuts)
, d_bolts_transferred(numBolts)
, d_winner_name(winner)
, d_loser_name(loser)
{
}

string BattleEvent::animate_event()
{
    stringstream ss;
    ss << fightingBotBots << endl;
    if (0 == d_nuts_transferred && 0 == d_bolts_transferred) {
        ss << d_loser_name << " escaped a battle with "
             << d_winner_name << "!" << endl;
    }
    else {
        ss << d_winner_name << " defeated " << d_loser_name << endl;
        ss << d_winner_name << " took " << d_nuts_transferred << " nuts "
           << "and " << d_bolts_transferred << " bolts from "
           << d_loser_name << endl;
    }
    return ss.str();
}

                            //====================
                            // class FoundNutEvent
                            //====================

FoundNutEvent::FoundNutEvent(int numNuts, string botName)
: d_numNuts(numNuts)
, d_botName(botName)
{
}

string FoundNutEvent::animate_event()
{
    stringstream ss;
    ss << happyBotBots << endl;
    ss << d_botName <<" found " << d_numNuts << " nuts!" << endl;
    return ss.str();
}

} // close namespace events
