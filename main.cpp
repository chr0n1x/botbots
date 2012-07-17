#include "grid.h"
#include "event.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>

#include <ncurses.h>

using namespace std;

namespace {
const int MAX_EVENTS_PER_SCREEN = 2;
int sleep_period = 1;

void printEvents(vector<events::Event*>& events)
{
    if (events.empty()) {
        return;
    }

    int numPrinted = 0;
    bool hasMessages = true;
    for(int i = 0; i < events.size(); ++i)
    {
        hasMessages = true;
        if (numPrinted % MAX_EVENTS_PER_SCREEN == 0 && 0 != numPrinted) {
            hasMessages = false;
            sleep(2*sleep_period);
            erase();
        }
        stringstream ss;
        ss << "Event " << numPrinted+1 << " of " << events.size() << endl;
        printw(ss.str().c_str());
        string eventStr = events[i]->animate_event();
        printw(eventStr.c_str());
        printw("\n-----------------------------------------\n");
        delete events[i];
        refresh();
        sleep(sleep_period);

        ++numPrinted;
    }

    if (hasMessages) {
        hasMessages = false;
        sleep(2*sleep_period);
        erase();
    }

    events.clear();
}

}

int main(int argc, char ** argv)
{
    srand( time(NULL) );

    int  grid_rows = 0;
    int  grid_cols = 0;
    bool rows_assigned = false;

    bool fish_tank = false;
    bool display_events = true;
    bool display_grid = true;

    int dummy = 0;
    if(argc >= 2) {
      for (int i = 1; i < argc; ++i)
      {
        dummy = atoi(argv[i]);

        if(dummy == 0) {
          string option(argv[i]);

          int dash_pos = option.find("-", 0);
          while(dash_pos != string::npos) {
              option.erase(dash_pos, 1);
              dash_pos = option.find("-", 0);
          }

          if (option == "fishtank" || option == "f") {
              fish_tank = true;
          }
          else if (option == "nosleep" || option == "ff") {
              sleep_period = 0;
          }
          else if (option == "noevent" || option == "ne") {
              display_events = false;
          }
          else if (option == "nogrid" || option == "ng") {
              display_grid = false;
          }
          else {
              cout << "Unrecognized option: " << option
                   << " ...stopping" << endl;
              return 0;
          }
        }
        else {
          if(rows_assigned) {
            grid_cols = atoi(argv[i]);
          }
          else {
            grid_rows = atoi(argv[i]);
            rows_assigned = true;
          }
        }
      }
    }

    the_grid::Grid g(grid_rows, grid_cols, fish_tank);
    cout << "Initializing Grid..." << endl;
    g.fill_to_capacity();

    vector<events::Event*> events;
    g.hold_events(&events);

    initscr();
    while(g.botbot_count() > 1)
    {
        erase();
        g.initiate_cycle();
        if (display_grid) {
            printw(g.to_string().c_str());
        }
        else {
            printw(g.stats_to_string().c_str());
        }
        refresh();
        sleep(sleep_period);
        clear();
        if (display_events) {
            printEvents(events);
        }
    }

    erase();
    if (display_grid) {
        printw(g.to_string().c_str());
    }
    else {
        printw(g.stats_to_string().c_str());
    }

    printw("\nPress any key to exit\n");

    cbreak();
    noecho();
    getch();

    refresh();

    endwin();

  return 0;
}
