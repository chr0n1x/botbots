#ifndef __BOTFACTORY
#define __BOTFACTORY

#include <iostream>
#include <string>
#include <cstdlib>
#include <pthread.h>

using namespace std;

namespace bot_factory {

  class botbot {
    int nuts, bolts, gen, grid_cycles;

    public:
      botbot() {
        srand( time(NULL) );
        nuts = rand() % 100;
        bolts = rand() % 100;
        grid_cycles = rand() % 100;
      }
      virtual ~botbot() {}

      virtual string name() {
        char namec[32] = {'\0'};
        sprintf(namec, "botbot_n%db%dgc%d-v%d", nuts, bolts, grid_cycles, gen);
        const char * cnamec = namec;
        string ret(cnamec);
        return ret;
      }

      virtual bool clash(botbot * hostile) {
        return false;
      }

      virtual void assign_gen(int in) {
        gen = in;
      }
  };

  class legacy_botbot : public botbot{
    int gen;
    pthread_mutex_t gen_lock;
    public:

      legacy_botbot() {
        gen = 0;
        pthread_mutex_init(&gen_lock, NULL);
      }
      ~legacy_botbot() {
        pthread_mutex_destroy(&gen_lock);
      }

      string name() {
        string ret("Legacy Bot v0.1");
        return ret;
      }

      bool clash(botbot * hostile) {
        if(hostile != NULL) delete hostile;
        return true;
      }

      int increase_generations() {
        pthread_mutex_lock(&gen_lock);
        ++gen;
        pthread_mutex_unlock(&gen_lock);
        return gen;
      }

      int generations() {
        cout << gen << endl;
        return gen;
      }

      void assign_gen(int in) {
        cout << "I AM LEGACY BOTBOT, DON'T FUCK WITH MY DATAS" << endl;
        return;
      }
  };

}

#endif
