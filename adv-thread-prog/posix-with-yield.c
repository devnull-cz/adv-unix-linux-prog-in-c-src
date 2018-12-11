/*
 * Show that we can work with both thread APIs in one program. We use POSIX API
 * to create threads, and Solaris API to yield (thr_yield()). Run with and
 * without -y, ^C it after a few seconds and see that the latter case will have
 * much less cycles in the thread 0.
 *
 * Note - you must have less CPUs than threads. If you have more CPUs then yield
 * will not help since you have no more threads to yield the CPU to.
 *
 * Also, see yield(2) on Solaris.
 *
 * (c) jp@devnull.cz
 */

#include <pthread.h>
#include <thread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

#define	NTHREADS	4

sigset_t mysigset;
int counters[NTHREADS];
int do_yield;

void
*thread(void *x)
{
	while (1) {
		/* Non-portable use. */
		++counters[(int)x];

		/* Yield only in the 1st thread. */
		if (do_yield == 1 && (int)x == 0)
			thr_yield();
	}
}

int
main(int argc, char **argv)
{
	int i, sig;
	pthread_t t;

	if (argc == 1)
		;
	else if (argc == 2 && strcmp("-y", argv[1]) == 0)
		do_yield = 1;
	else
		errx(1, "Bad usage.");

	printf("Use -y to trigger yield call.\n");

	sigfillset(&mysigset);
	pthread_sigmask(SIG_SETMASK, &mysigset, NULL);

	/* Non-portable use of passing the ID. */
	for (i = 0; i < NTHREADS; ++i) {
		pthread_create(&t, NULL, thread, (void *)i);
	}

	/* See sigwait() for more information on number of params. */
	sig = sigwait(&mysigset);
	printf("---> caught signal #%d\n", sig);
	for (i = 0; i < NTHREADS; ++i) {
		printf("counter %d: %d\n", i, counters[i]);
	}

	return (0);
}
