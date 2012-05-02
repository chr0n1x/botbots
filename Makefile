all:
	make botbots; make ct;

botbots:
	g++ main.cpp -g -lncurses -lpthread -o botbots;

ct:
	g++ cortex_test.cpp -lpthread -g -o cortex_test;

clean:
	rm -rf botbots cortex_test *.dSYM
