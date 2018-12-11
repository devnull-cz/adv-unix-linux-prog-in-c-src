
PROG=bind

all: $(PROG)

$(PROG): cycles.c
	gcc -o $(PROG) $? -lnsl -lsocket

clean:
	rm -f *.o $(PROG)
