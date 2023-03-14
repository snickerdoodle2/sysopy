all:
	make clean
	make small
	make large

small:
	gcc -Wall small.c -o small

large:
	gcc -Wall large.c -o large

clean:
	rm -f small large

test:
	make all
	./small in.txt tmp1.txt
	./small tmp1.txt tmp2.txt
	diff -s tmp2.txt in.txt
	./large in.txt tmp1.txt
	./large tmp1.txt tmp2.txt
	diff -s tmp2.txt in.txt
	rm tmp1.txt tmp2.txt