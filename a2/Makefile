sequential:
	gcc 1a.c -o client
	gcc 2a.c -o server
	./server

concurrent-fork:
	gcc 1b.c -o client
	gcc 2b.c -o server
	./server

concurrent-thread:
	gcc 1b.c -o client
	gcc 2c.c -o server
	./server

nonblocking-select:
	gcc 1b.c -o client
	gcc 2di.c -o server
	./server

nonblocking-poll:
	gcc 1b.c -o client
	gcc 2dii.c -o server
	./server
