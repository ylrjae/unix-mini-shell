CC =  gcc
CFLAGS = -Wall -Wextra -g 

http_server: server.c
	$(CC) $(CFLAGS) -o server server.c

run: server
	./server


clean:
	rm -f server

