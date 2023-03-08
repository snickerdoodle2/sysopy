static:
	gcc -Wall -c my_lib.c
	ar crs libmy_lib.a my_lib.o
	rm -f *.o libmy_lib.a

shared:
	gcc -Wall $(FLAGS) -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	rm -f *.o

dynamic:
	gcc -fPIC -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	rm -f *.o


all:
	make static
	make shared
	make dynamic

clean:
	rm -f *.o *.a *.so main
