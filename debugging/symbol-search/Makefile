
all: bigfoo.so.1 bigbar.so.1 prog

smallfoo.so.1: smallfoo.c
	gcc -o smallfoo.so.1 -G -fPIC smallfoo.c

smallbar.so.1: smallbar.c
	gcc -o smallbar.so.1 -G -fPIC smallbar.c

bigfoo.so.1: smallfoo.so.1 bigfoo.c
	gcc -o bigfoo.so.1 -G -fPIC bigfoo.c -R. smallfoo.so.1

#bigbar.so.1: smallbar.so.1 bigbar.c
#	gcc -o bigbar.so.1 -G -fPIC bigbar.c -R. smallbar.so.1
bigbar.so.1: smallbar.so.1 bigbar.c
	gcc -o bigbar.so.1 -G -fPIC bigbar.c -R. smallfoo.so.1

bigbar-direct.so.1: smallbar.so.1 bigbar.c smallfoo.so.1
	gcc -c bigbar.c
	ld -o bigbar-direct.so.1 -G -R. -Bdirect smallfoo.so.1 bigbar.o

prog: main.c bigfoo.so.1 bigbar.so.1
	gcc -o prog -R. main.c bigfoo.so.1 bigbar.so.1

directprog: main.c bigfoo.so.1 bigbar-direct.so.1
	gcc -o directprog -R. main.c bigfoo.so.1 bigbar-direct.so.1

clean:
	rm -f *.so.1 prog
