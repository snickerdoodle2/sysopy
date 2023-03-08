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


test_A_B:
	make $(TYPE)
	./main < commands.txt > results/results_$(TYPE)_$(FLAGS).txt

all_opt:
	make test_A_B TYPE=static
	make test_A_B TYPE=shared
	make test_A_B TYPE=dynamic

test:
	rm -rf results
	mkdir results
	make all_opt FLAGS="-O0"
	make all_opt FLAGS="-O2"
	make all_opt FLAGS="-Os"

clean:
	rm -f *.o *.a *.so main
