all:
# biblioteka statyczna
	gcc -c bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c main.c
	gcc main.o libbibl1.a -o main_stat
# biblioteka współdzielona
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc -c main.c
	gcc main.o -L. -lbibl1 -Wl,-rpath=. -o main_wspoldz
# biblioteka ladowana dynamicznie przez program
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc main2.c -o main_ladow_dynam -ldl