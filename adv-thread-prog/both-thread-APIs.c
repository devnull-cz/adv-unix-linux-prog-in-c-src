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
 * A simple example on using both Solaris and POSIX thread API. Note that if we
 * create a thread using one API we must join it using the same API since the
 * thread types are not compatible (thread_t versus pthread_t).
 */

#include <stdio.h>
#include <unistd.h>
#include <thread.h>
#include <pthread.h>

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
	pthread_t ptid;

	/* Create a thread using the Solaris Threads API call. */
	thr_create(NULL, 0, run_thread, (void *)1, 0, &tid);
	/* Now do the same with POSIX API. */
	pthread_create(&ptid, NULL, run_thread, (void *)2);

	/* Just wait for one of those. */
	thr_join(tid, NULL, NULL);
	printf("\n");
}
