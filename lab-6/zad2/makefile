all:
	make clean
	gcc -Wall -o server server.c -lrt
	gcc -Wall -o client client.c -lrt

clean:
	rm -f server client logs.txt