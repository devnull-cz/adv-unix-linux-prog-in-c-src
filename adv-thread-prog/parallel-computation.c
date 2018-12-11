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
 * Copyright 2010 Jan Pechanec, Vladimir Kotal.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Start argv[1] number of threads and let them loop for argv[2] seconds. Sum
 * all the accumulated counters and print the result. This program is intended
 * to help ascertain how much in paralel we can run on various multi CPU
 * machines.
 *
 * For example, on a uniprocesor, running the program with different number of
 * threads but the same number of seconds should yield rougly the same final
 * sum. To give you an idea, this is how it can be used on UltraSPARC T1 machine
 * with 6 cores (24 virtual CPUs). The following shows that if we run 6 threads
 * it linearly scales:
 *
 * $ ./a.out 6 10
 * 1661374469
 * $ ./a.out 1 10
 * 277845608
 * $ bc
 * 6*277845608
 * 1667073648
 *
 * With 24 threads we can see that virtual CPUs are not fully independent and
 * that we get "only" 70% of the theoretical maximum:
 *
 * $ ./a.out -g 24 10
 * 4679205463
 * ./a.out 1 10
 * 277776481
 * $ bc
 * 277776481 * 24
 * 6666635544
 * scale=2
 * 4679205463/6666635544
 * .70
 *
 * BTW, use /usr/sbin/psrinfo on Solaris to get information on installed CPUs.
 */

#include <pthread.h>
#include <libgen.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define	MAX_THREADS	256

/*
 * Use if err(3) family is not present. Do not forget to comment out <err.h>
 * then.
 */
#if 0
#define errx(a, b, c)       printf(b, c); printf("\n"); exit(a)
#endif

int done;
int global_memory;
long long counters[MAX_THREADS];

void
usage(char *argv0)
{
	errx(1, "usage: %s [-g] <n_threads> <seconds>\n"
	    "  \"-g\" will make the program to use global memory for counters",
	    basename(argv0));
}

void *
worker(void *arg)
{
	long long *ctr = NULL;
	long long counter = 0;

	if (global_memory == 1)
		ctr = (long long *)arg;
	else
		ctr = &counter;

	while (1) {
		(*ctr)++;
		if (done == 1)
			break;
	}

	if (global_memory == 0)
		*((long long *)arg) = *ctr;
	return (NULL);
}

int
main(int argc, char **argv)
{
	int err, i, n, secs;
	pthread_t thr[MAX_THREADS];

	if (strcmp(argv[1], "-g") == 0) {
		if (argc != 4)
			usage(argv[0]);
		global_memory = 1;
		printf("Using global memory for counters.\n");
	}
	else {
		if (argc != 3)
			usage(argv[0]);
		printf("Using local memory for counters (use -g for global).\n");
	}

	if ((n = atoi(argv[1 + global_memory])) > MAX_THREADS) {
		errx(1, "Number of threads can not be greater than %d.",
		    MAX_THREADS);
	}

	secs = atoi(argv[2 + global_memory]);

	for (i = 0; i < n; ++i) {
		if ((err = pthread_create(thr + i, NULL, worker, counters + i))
		    != 0) {
			errx(1, "pthread_create: %s", strerror(err));
		}
	}

	sleep(secs);
	done = 1;
	
	for (i = 0; i < n; ++i) {
		pthread_join(thr[i], NULL);
		printf("%lld\n", counters[i]);
	}

	if (n > 1) {
		long long sum = 0;
		for (i = 0; i < n; ++i)
			sum = sum + counters[i];
		printf("---\n%lld\n", sum);
	}

	return (0);
}
