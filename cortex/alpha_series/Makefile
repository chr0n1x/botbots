all:
	make botbots; make ct; make jobtest;

#botbots: botbot.cpp grid.cpp main.cpp
#	g++ main.cpp botbot.cpp grid.cpp -g -lncurses -lpthread -o botbots;

botbots: botbot.o grid.o main.cpp
	g++ main.cpp botbot.o grid.o -g -lncurses -lpthread -o botbots;

grid.o: grid.h grid.cpp
	g++ grid.cpp -g -lpthread -c

botbot.o: botbot.h botbot.cpp
	g++ botbot.cpp -g -lpthread -c

ct: cortex.o task.o boundjob.o condition_variable.o mutex.o
	g++ -g -pthread mutex.o condition_variable.o cortex.o task.o \
        boundjob.o cortex.t.cpp -o cortex_test -lm;

mutex.o: mutex.cpp mutex.h timer.o
	g++ -g -c mutex.cpp

condition_variable.o: condition_variable.cpp condition_variable.h
	g++ -g -c condition_variable.cpp

cortex.o: cortex.cpp cortex.h
	g++ -g -c cortex.cpp

timer.o: timer.cpp timer.h
	g++ -g -c timer.cpp

jobtest: boundjob.o task.o
	g++ -g boundjob.o task.o boundjob.t.cpp -o boundjob_test

boundjob.o: boundjob.cpp boundjob.h task.o
	g++ -g -c boundjob.cpp

task.o: task.cpp task.h
	g++ -g -c task.cpp 

clean:
	rm -rf botbots cortex_test boundjob_test *.dSYM *.o
