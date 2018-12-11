/*
 * A simple example of using the Solaris thread API. The program runs 3 threads,
 * each thread prints its ID every second, for 10 seconds.
 */

#include <thread.h>
#include <stdio.h>
#include <unistd.h>

void *
run_thread(void *arg)
{
	int i;

	printf("thread %d started\n", (int)arg);

	for (i = 0; i < 10; ++i) {
		sleep(1);
		printf("%d", (int)arg);
		fflush(stdout);
	}

	return (NULL);
}

int
main(void)
{
	thread_t tid;

	/* possibly non-portable ID passing */
	thr_create(NULL, 0, run_thread, (void *)1, 0, &tid);
	thr_create(NULL, 0, run_thread, (void *)2, 0, &tid);
	thr_create(NULL, 0, run_thread, (void *)3, 0, &tid);

	thr_join(tid, NULL, NULL);
	printf("\n");
}
