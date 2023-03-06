static:
	make clear
	gcc -Wall -c my_lib.c
	ar crs libmy_lib.a my_lib.o
	rm -f *.o

dynamic:
	make clear
	gcc -Wall -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	rm -f *.o

clear:
	rm -f *.o *.a *.so lib1