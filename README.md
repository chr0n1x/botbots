-- SETUP --

YOU CAN FIND INFORMATION ON NCURSES HERE: http://www.gnu.org/software/ncurses/

* Go into ncurses and install if you do not have it already:
  * cd ncurses-5.9
  * ./configure
  * make
  * sudo make install

* Back to parent directory
  * make botbots
  * ./botbots

NOTE: If you have trouble compiling, try changing the -lpthread flags to -pthread
if you're getting undefined references to pthread_create and similar functions

-- ETC --

The default dimensions of the grid itself are currently 5x5.
The main interest of this project at the moment is the thread queue that can take any kind of object. It started with me just wanted to play with threads.
I just ended up created an ADT-like object called the Cortex, which takes specialized_cortex_objects<> that inherit properties from a generic cortex_object class.

Fun and games start there. Things are still in progress.
This project is more for the sake of learning than it is for any practical application (although I might use my cortex for other projects?). Maybe other ideas will arise during its development.

Find any bugs? SEGFAULTs? Suggestions? Email me at heilong24@gmail.com

-- UPDATE 7/27/2012 - ALPHA SERIES --

Alpha series of the Cortex is ready to use! It does run faster than STL::queue and is our own implementation of POSIX threads.
Compared to a standard implementation of POSIX threads, we have a 25% temporal overhead for class inits, mallocs, etc (looking into ways to reduce this in the Beta Series)
Using this class though would eliminate the need for you to look into your own ways to block threads and pass tasks to a pool of workers. Just bind a function and arguments
to an element and push it into the queue, the Cortex takes care of the rest (look at the cortex test - cortex.t.cpp - for an example).

-- THANKS --

genKeen - friend and teacher. Gonna make a man outta me by teaching me more C++
aakside - friend and fellow dreamer. We're gonna get somewhere, someday, bro.
