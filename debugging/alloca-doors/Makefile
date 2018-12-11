
PROGS=server-alloca server-malloc client

all: $(PROGS)

server-malloc: config.h door-srv.c 
	gcc -DMALLOC -o server-malloc door-srv.c -ldoor

server-alloca: config.h door-srv.c 
	gcc -o server-alloca door-srv.c -ldoor

client: config.h door-cli.c
	gcc -o client door-cli.c -ldoor

clean:
	rm -f *.o core core.* $(PROGS)
