CC=cc

PROG=run
LIB=libfill.so.1

all: $(PROG)

$(LIB): fill.c
	$(CC) -shared -o $(LIB) fill.c

$(PROG): main.c $(LIB)
	$(CC) -o $(PROG) main.c -L. -lfill

clean:
	rm -f *.o core.* core $(LIB)
