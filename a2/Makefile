sequential:
	mkdir -p output
	gcc 1a.c -o client
	gcc 2a.c -o server
	./server

concurrent-fork:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2b.c -o server
	./server

concurrent-thread:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2c.c -o server
	./server

nonblocking-select:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2di.c -o server
	./server

nonblocking-poll:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2dii.c -o server
	./server

nonblocking-epoll:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2diii.c -o server
	./server