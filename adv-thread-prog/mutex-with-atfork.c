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
 * Program forks while one of the thread has the mutex locked. Use -h to use
 * at-fork handlers. It can take some time before the at-fork prepare handler
 * get hold of the mutex. Do not get confused with the "mutex locked" message
 * after the fork and the child reporting that it acquired the mutex as well.
 * The message is from the parent - remember we have not threads but the main
 * one in the child.
 */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <sys/wait.h>

/* Let's say this is mutex that synchronizes access to some common data. */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void
*thread(void *x)
{
	int ret, id = (int)x;

	/*
	 * Each thread just do something under the protection of the mutex.
	 *
	 * Interestingly, those 2 threads loop for some time without the prepare
	 * handler geting chance of locking the mutex. I do not know why but I
	 * see such behaviour on OpenSolaris, FreeBSD 7.2 and Linux 2.6.22. As
	 * if it took some time for the main() to get the priority to run again
	 * after it spent some resources on creating the threads.
	 */
	while (1) {
		if ((ret = pthread_mutex_lock(&mutex)) > 0)
			errx(1, "%d: pthread_mutex_lock: %s.\n", id,
			    strerror(ret));
		fprintf(stderr, "%d: mutex locked.\n", id);
		sleep(1);
		if ((ret = pthread_mutex_unlock(&mutex)) > 0)
			errx(1, "%d: pthread_mutex_unlock: %s.\n", id,
			    strerror(ret));
		fprintf(stderr, "%d: mutex unlocked.\n", id);
	}

	/* Not reached. */
	return (NULL);
}

void
prepare(void)
{
	int ret;

	fprintf(stderr, "Main trying to acquire the lock before fork().\n");
	if ((ret = pthread_mutex_lock(&mutex)) > 0)
		errx(1, "pthread_mutex_lock: %s.\n", strerror(ret));
	fprintf(stderr, "Main done with pre-fork handling.\n");
}

void
parent(void)
{
	int ret;

	if ((ret = pthread_mutex_unlock(&mutex)) > 0)
		errx(1, "pthread_mutex_unlock: %s.\n", strerror(ret));
	fprintf(stderr, "Parent done in the post-fork handler.\n");
}

void
child(void)
{
	int ret;

	if ((ret = pthread_mutex_unlock(&mutex)) > 0)
		errx(1, "pthread_mutex_unlock: %s.\n", strerror(ret));
	fprintf(stderr, "Child done in the post-fork handler.\n");
}

int
main(int argc, char **argv)
{
	pthread_t t1, t2;

	fprintf(stderr, "PID is %d.\n", getpid());

	if (argc > 1 && strcmp(argv[1], "-h") == 0) {
		fprintf(stderr, "Running with the atfork handlers.\n");
		if (pthread_atfork(prepare, parent, child) != 0)
			err(1, "pthread_atfork");
	}
	else
		fprintf(stderr, "Running WITHOUT the atfork handlers (use "
		    "-h for that).\n");

	pthread_create(&t1, NULL, thread, (void *)0);
	pthread_create(&t2, NULL, thread, (void *)1);

	/*
	 * Make sure we will not try to lock the mutex first otherwise we would
	 * have nothing to show here. So sleep for a little while.
	 *
	 * Interestingly, on Linux 2.6.22 without the following sleep(1) call in
	 * in the default case (w/o the -h option) I can see that the mutex is
	 * locked by one of the thread but pthread_mutex_trylock() will return
	 * OK anyway.
	 *
	 * I can see a similar problem on FreeBSD 7.2 with the non-default
	 * threading library, libkse (it's M:N). When the -h option is used,
	 * at-fork handlers are done, the lock is unlocked in the child but the
	 * child still reports that the mutex seems to be locked.
	 */
	sleep(1);

	/* Child. */
	fprintf(stderr, "Will fork now.\n");
	if (fork() == 0) {
		int ret;

		fprintf(stderr, "This is the child.\n");
		fprintf(stderr, "Will try to get hold of the mutex.\n");
		ret = pthread_mutex_trylock(&mutex);
		/*
		 * On locked mutex, pthread_mutex_trylock() returns EBUSY
		 * according to the POSIX spec.
		 */
		if (ret == EBUSY)
			fprintf(stderr, "The mutex is reported locked and "
			    "there is no way for me to unlock it.\n");
		else if (ret > 0)
			errx(1, "pthread_mutex_trylock: %s.\n", strerror(ret));
		else
			fprintf(stderr, "Mutex could be successfully "
			    "acquired.\n");
		fprintf(stderr, "Child exiting.\n");
		return (0);
	}

	fprintf(stderr, "This is the parent.\n");
	wait(NULL);
	fprintf(stderr, "Parent exiting.\n");

	return (0);
}
