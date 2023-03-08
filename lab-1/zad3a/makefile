static:
	make clean
	gcc -Wall -c my_lib.c
	ar crs libmy_lib.a my_lib.o
	gcc -Wall main.c -o main -L. -lmy_lib
	rm -f *.o libmy_lib.a

shared:
	make clean
	gcc -Wall $(FLAGS) -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	gcc main.c -L. -lmy_lib -Wl,-rpath=. -o main
	rm -f *.o

dynamic:
	make clean
	gcc -fPIC -c my_lib.c
	gcc -Wall -shared -fPIC -o libmy_lib.so my_lib.o
	gcc main2.c -o main -ldl
	rm -f *.o

test_A:
	make $(TYPE)
	./main < commands.txt > results/results_$(TYPE).txt
	make clean

all:
	make test_A TYPE=static
	make test_A TYPE=shared
	make test_A TYPE=dynamic

test:
	rm -rf results
	mkdir results
	make all
	
clean:
	rm -f *.o *.a *.so main
