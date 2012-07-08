all:
	make cortex_test; make task_test;

cortex_test:
	g++ -g cortex.t.cpp cortex.cpp task.cpp -lpthread -o cortex_test;

task_test:
	g++ -g task.t.cpp task.cpp task.cpp -lpthread -o task_test;

sync_test:
	g++ -g mutex.cpp condition_variable.cpp synchronize.t.cpp -lpthread -o sync_task

clean:
	rm sync_test *.exe *.o
