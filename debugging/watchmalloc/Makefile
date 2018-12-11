CC=cc

PROG=underoverflow
PROG1=badfree

PROGS=$(PROG) $(PROG1)

all: $(PROGS)

$(PROG): buffer.c
	$(CC) -o $(PROG) $?

$(PROG1): free.c
	$(CC) -o $(PROG1) $?

clean:
	rm -f *.o core.* core $(PROGS)
