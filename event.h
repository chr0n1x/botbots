#ifndef INCLUDED_EVENT_H
#define INCLUDED_EVENT_H

#include <string>

using namespace std;

namespace events {

                            //============
                            // class Event
                            //============
class Event
{
  public:

    virtual ~Event();

    virtual string animate_event() = 0;
};

                            //==================
                            // class BattleEvent
                            //==================

class BattleEvent : public Event
{
    // PRIVATE DATA
    int    d_nuts_transferred;  // nuts lost/won
    int    d_bolts_transferred; // bolts lost/won
    string d_winner_name;
    string d_loser_name;

  public:

    // CREATORS
    BattleEvent(int numNuts,
                int numBolts,
                const string& winner,
                const string& loser);

    virtual string animate_event();
};

                            //====================
                            // class FoundNutEvent
                            //====================

class FoundNutEvent : public Event
{
    // PRIVATE DATA
    int    d_numNuts; // hehe, numb nuts
    string d_botName;     // bot which found nuts (held)

  public:

    // CREATORS
    FoundNutEvent(int numNuts, string botName);

    virtual string animate_event();
};

} // close namespace events

#endif
