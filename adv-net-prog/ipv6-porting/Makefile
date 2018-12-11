
PROG=getaddr

all: $(PROG)

$(PROG): legacy.c
	gcc -o $(PROG) $? -lnsl -lsocket

clean:
	rm -f *.o $(PROG)
