LIBS=sema.c shared.c

all:
	make clean
	gcc -Wall -o main $(LIBS) main.c
	gcc -Wall -o client $(LIBS) client.c
	gcc -Wall -o barber $(LIBS) barber.c

clean:
	rm -f client barber main
