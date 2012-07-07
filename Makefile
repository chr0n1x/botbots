all:
	make clean; make botbots; make ct; make jobtest;

botbots: botbot.cpp grid.cpp main.cpp event.o
	g++ main.cpp botbot.cpp grid.cpp event.o \
        -g -lncurses -lpthread -o botbots;

ct: cortex.o task.o boundjob.o
	g++ -g -lpthread cortex.o task.o boundjob.o cortex.t.cpp -o cortex_test;

cortex.o: cortex.cpp cortex.h
	g++ -g -c cortex.cpp

jobtest: boundjob.o task.o
	g++ -g boundjob.o task.o boundjob.t.cpp -o boundjob_test

boundjob.o: boundjob.cpp boundjob.h task.o event.o
	g++ -g -c boundjob.cpp

task.o: task.cpp task.h
	g++ -g -c task.cpp 

event.o: event.cpp event.h
	g++ -g -c event.cpp

clean:
	rm -rf botbots cortex_test boundjob_test *.dSYM *.o
