all:
	make clean; make botbots; make ct; make jobtest;

botbots: botbot.cpp grid.cpp main.cpp
	g++ main.cpp botbot.cpp grid.cpp -g -lncurses -lpthread -o botbots;

#botbots: botbot.o grid.o main.o
#	g++ main.o botbot.o grid.o -g -lncurses -lpthread -o botbots;

#grid.o: grid.h grid.cpp
#	g++ grid.cpp -g -lpthread -c

#botbot.o: botbot.h botbot.cpp
#	g++ botbot.cpp -g -lpthread -c

#main.o: main.cpp
#	g++ main.cpp -g -lpthread -c

# WAT
ct: cortex.o task.o boundjob.o
	g++ -g -lpthread cortex.o task.o boundjob.o cortex.t.cpp -o cortex_test;

jobtest:
	g++ -g boundjob.t.cpp task.cpp -o boundjob_test;

clean:
	rm -rf botbots cortex_test boundjob_test *.dSYM
