all:
	make clean; make botbots; make ct; make jobtest;

botbots:
	g++ main.cpp -g -lncurses -lpthread -o botbots;

ct:
	g++ cortex_test.cpp -lpthread -g -o cortex_test;

jobtest:
	g++ -g boundjob.t.cpp task.cpp -o boundjob_test;

clean:
	rm -rf botbots cortex_test *.dSYM
