static:
		make clear
		gcc -Wall -c my_lib.c
		ar crs libmy_lib.a my_lib.o
		gcc -Wall main.c -o main -L. -lmy_lib
		rm -f *.o

shared:
		make clear
		gcc -Wall -c my_lib.c
		gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
		rm -f *.o

dynamic:
	make clear
	gcc -fPIC -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	gcc main2.c -o main -ldl



clear:
		rm -f *.o *.a *.so main