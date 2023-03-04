static:
	make clear
	gcc -Wall -c zad1.c
	ar crs libzad1.a zad1.o
	rm -f *.o

dynamic:
	make clear
	gcc -Wall -c zad1.c
	gcc -Wall -shared -fPIC -o libsay.so zad1.o
	rm -f *.o

clear:
	rm -f *.o *.a *.so lib1