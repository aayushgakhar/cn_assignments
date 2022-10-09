sequential:
	mkdir -p output
	gcc 1a.c -o client
	gcc 2a.c -o server
	./server

fork:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2b.c -o server
	./server

thread:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2c.c -o server
	./server

select:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2di.c -o server
	./server

poll:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2dii.c -o server
	./server

epoll:
	mkdir -p output
	gcc 1b.c -o client
	gcc 2diii.c -o server
	./server