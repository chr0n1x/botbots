all:
	g++ main.cpp -g -lncurses -lpthread -o botbots

clean:
	rm -rf botbots botbots.dSYM
