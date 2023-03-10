lib:
	gcc -Wall lib.c -o lib 

sys:
	gcc -Wall sys.c -o sys 

all:
	make clean
	make lib
	make sys

test:
	make all
	{ time ./lib o z in.txt out.txt; } 2> pomiar_zad_1.txt
	{ time ./sys o z in.txt out.txt; } 2>> pomiar_zad_1.txt

clean:
	rm -f lib sys 
