static:
	gcc -Wall -c my_lib.c
	ar crs libmy_lib.a my_lib.o
	gcc -Wall main.c -o main -L. -lmy_lib
	rm -f *.o

shared:
	gcc -Wall -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	gcc main.c -L. -lmy_lib -Wl,-rpath=. -o main
	rm -f *.o

dynamic:
	gcc -fPIC -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	gcc main2.c -o main -ldl

all:
	make static
	make shared
	make dynamic

clean:
	rm -f *.o *.a *.so main
