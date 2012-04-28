all:
	g++ main.cpp -g -lncurses -lpthread -o botbots

clean:
	rm botbots
