/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright 2015 Jan Pechanec, Vladimir Kotal.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Example door server.  Every time the server function is called an internal
 * counter incremented by one is returned.  The program has no parameters and
 * the door itself is attached to a file from argv[1]. The file is created if
 * needed.  Run it like this:
 *
 *  $ ./server /tmp/door_xxx
 *
 * and then run the client "./client /tmp/door_xxx"
 */

#include <fcntl.h>
#include <err.h>
#include <stdio.h>
#include <door.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic.h>

void
server(void *cookie, char *argp, size_t arg_size, door_desc_t *dp,
    uint_t n_desc)
{
	static unsigned int counter = 0;

	/*
	 * For every client door request a new thread is created.  We must
	 * synchronize or make it atomic!  You could just use "++counter" and
	 * then try on SPARC to see what happens if multiple clients in a loop
	 * access the server.  There will be a race.
	 */
	atomic_add_int(&counter, 1);
	(void) door_return((char *)&counter, sizeof (int), NULL, 0);
}

int
main(int argc, char **argv)
{
	int doorid;

        if (argc != 2)
                errx(1, "bad params");

	if ((doorid = door_create(server, 0, 0)) == -1)
		err(1, "door_create");

	/* you should not do this in a production environment */
	(void) close(open(argv[1], O_RDONLY | O_CREAT, 0644));
	/* Clean up in case it was used before. */
	(void) fdetach(argv[1]);

	/* Connect the existing door with a specific filename. */
	if (fattach(doorid, argv[1]) == -1)
		err(2, "fattach");

	for ( ; ; )
		(void) pause();

#pragma error_messages (off, E_STATEMENT_NOT_REACHED)
	/* NOTREACHED */
	return (0);
}
