dynamic:
	gcc -fPIC -c bibl1.c
	gcc -Wall -shared -fPIC -o libbibl1.so bibl1.o
	gcc zad1.c -o zad1 -ldl

clean:
	rm -f *.o