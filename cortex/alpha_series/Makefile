#DEBUG=-g
DEBUG=-O2

all:
	make botbots; make ct; make jobtest;

#botbots: botbot.cpp grid.cpp main.cpp
#	g++ main.cpp botbot.cpp grid.cpp -g -lncurses -lpthread -o botbots;

botbots: botbot.o grid.o main.cpp
	g++ main.cpp botbot.o grid.o $(DEBUG) -lncurses -lpthread -o botbots;

grid.o: grid.h grid.cpp
	g++ grid.cpp $(DEBUG) -lpthread -c

botbot.o: botbot.h botbot.cpp
	g++ botbot.cpp $(DEBUG) -lpthread -c

ct: cortex.o task.o boundjob.o condition_variable.o mutex.o \
    multiqueue_threadpool.o
	g++ $(DEBUG) -lpthread mutex.o condition_variable.o cortex.o task.o \
        boundjob.o multiqueue_threadpool.o cortex.t.cpp -o cortex_test -lm;

mutex.o: mutex.cpp mutex.h
	g++ $(DEBUG) -c mutex.cpp

condition_variable.o: condition_variable.cpp condition_variable.h
	g++ $(DEBUG) -c condition_variable.cpp

cortex.o: cortex.cpp cortex.h mutex.o boundjob.o
	g++ $(DEBUG) -c cortex.cpp

mtqt: multiqueue_threadpool.o multiqueue_threadpool.t.cpp synch_semaphore.o \
		task.o boundjob.o
	g++ $(DEBUG) -pthread multiqueue_threadpool.o synch_semaphore.o task.o \
				 boundjob.o multiqueue_threadpool.t.cpp -o mtq_test

multiqueue_threadpool.o: multiqueue_threadpool.h multiqueue_threadpool.cpp 
	g++ $(DEBUG) -c multiqueue_threadpool.cpp

timer.o: timer.cpp timer.h
	g++ $(DEBUG) -c timer.cpp

synch_semaphore.o: synch_semaphore.cpp synch_semaphore.h
	g++ $(DEBUG) -c synch_semaphore.cpp

jobtest: boundjob.o task.o
	g++ $(DEBUG) boundjob.o task.o boundjob.t.cpp -o boundjob_test

boundjob.o: boundjob.cpp boundjob.h task.o
	g++ $(DEBUG) -c boundjob.cpp

task.o: task.cpp task.h
	g++ $(DEBUG) -c task.cpp 

clean:
	rm -rf botbots cortex_test boundjob_test *.dSYM *.o
