
PROG=leaky

all: $(PROG)

$(PROG): reload.c
	cc -o $(PROG) $?

clean:
	rm -f *.o core core.* $(PROG)
