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
 * Copyright 2009 Jan Pechanec, Vladimir Kotal.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * This program is to show you one of the fork-one issues. The main thread
 * creates some threads, each thread allocates some memory. Fork in main().
 * mdb(1) will show you that we will have memory leaks in the child since the
 * memory pointers were on the stack of threads that are not duplicated in the
 * child. If we can't get those pointers we can't free the memory...
 *
 * 	$ LD_PRELOAD=/usr/lib/libumem.so.1 UMEM_DEBUG=default ./a.out
 *	PID: 6330
 *	child's PID: 6331
 *
 *	# Use gcore(1) from another terminal. You can NOT use SIGQUIT signal
 *	# here (Ctrl-\) since you would kill the father, not the child.
 *	# Remember, father has not memory leaks.
 *	$ gcore 6331
 *	gcore: core.6331 dumped
 *
 *	$ mdb core.6331
 *	Loading modules: [ libumem.so.1 ld.so.1 ]
 *	> ::findleaks
 *	CACHE     LEAKED   BUFCTL CALLER
 *	0806ac10      10 08078080 runthr+0x16
 *	------------------------------------------------------------------------
 *	   Total      10 buffers, 1120 bytes
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>

#define	MAX_THREADS	10

void *
runthr(void *x)
{
	char *p = NULL;

	if ((p = malloc(100)) == NULL)
		err(1, "malloc");

	sleep(99);
}

int
main(int argc, char **argv)
{
	pthread_t t;
	int i, ret;

	printf("PID: %d\n", getpid());

	for (i = 0; i < MAX_THREADS; ++i) {
		ret = pthread_create(&t, NULL, runthr, NULL);
		if (ret != 0)
			errx("pthread_create(): %s\n", strerror(ret));
	}

	/* Hopefully all threads are after malloc(3c) call after this call. */
	sleep(1);

	printf("gcore(1) me now...\n");

	/* Child. */
	if (fork() == 0) {
		printf("child's PID: %d\n", getpid());
		sleep(99);
	}
	else
		wait(NULL);

	printf("Exiting: %d\n", getpid());
	return (0);
}
