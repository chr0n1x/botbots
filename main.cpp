#include "grid.h"
#include "event.h"

#include <iostream>
#include <vector>
#include <cstdlib>

#include <ncurses.h>

using namespace std;

int main(int argc, char ** argv)
{
    srand( time(NULL) );

    int grid_rows = 0;
    int grid_cols = 0;

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
    the_grid::grid g(grid_rows, grid_cols, fish_tank);
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
       sleep(1);
       for(int i = 0; i < events.size(); ++i)
       {
           erase();
           string eventStr = events[i]->animate_event();
           printw(eventStr.c_str());
           delete events[i];
           refresh();
           sleep(3);
       }
       events.clear();
       refresh();
    }
    endwin();

  return 0;
}
