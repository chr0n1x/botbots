#include "grid.h"
#include "event.h"

#include <iostream>
#include <vector>
#include <cstdlib>

#include <ncurses.h>

using namespace std;

namespace {
const int MAX_EVENTS_PER_SCREEN = 2;

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
            sleep(2);
            erase();
        }
        string eventStr = events[i]->animate_event();
        printw(eventStr.c_str());
        printw("\n-----------------------------------------\n");
        delete events[i];
        refresh();
        sleep(1);

        ++numPrinted;
    }

    if (hasMessages) {
        hasMessages = false;
        sleep(2);
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
    bool fish_tank = false;

    if(argc >= 3) {
        grid_rows = atoi(argv[1]);
        grid_cols = atoi(argv[2]);
        if (argc == 4) {
            string option(argv[3]);

            int dash_pos = option.find("-", 0);
            while(dash_pos != string::npos) {
                option.erase(dash_pos, 1);
                dash_pos = option.find("-", 0);
            }

            fish_tank =
                option.compare("fishtank") == 0 || option.compare("f") == 0;

            if(!fish_tank) {
                cout << "Unrecognized option: " << option
                     << " ...stopping" << endl;
                return 0;
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
        printw(g.to_string().c_str());
        refresh();
        sleep(1);
        clear();
        printEvents(events);
    }

    erase();
    printw(g.to_string().c_str());

    printw("\nPress any key to exit\n");

    cbreak();
    noecho();
    getch();

    refresh();

    endwin();

  return 0;
}
