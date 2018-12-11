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

#include <err.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>

#define	BUFLEN	128

/*
 * Create a new process and read its standard output (of the child) from a pipe.
 * Print the read input (in the parent) on the standard output (of the parent).
 */
int
main(void)
{
	pid_t pid;
	char *argv[2];
	char buf[BUFLEN + 1];
	int n, flags, ret, p[2];
	posix_spawn_file_actions_t factions;

	if (pipe(p) == -1)
		err(1, "pipe");

	/*
	 * We intentionally mix FD_CLOEXEC with file actions to show that it
	 * works together.
	 */
	flags = fcntl(p[0], F_GETFL);
	if (fcntl(p[0], F_SETFL, flags | FD_CLOEXEC) == -1)
		err(1, "fcntl on %d", p[0]);

	flags = fcntl(1, F_GETFL);

	if ((ret = posix_spawn_file_actions_init(&factions)) != 0)
		errx(1, "posix_spawn_file_actions_init(): %s", strerror(ret));

	/* Duplicate the standard input to the pipe. */
	if ((ret = posix_spawn_file_actions_adddup2(&factions, p[1], 1)) != 0)
		errx(1, "posix_spawn_file_actions_adddup2(): %s", strerror(ret));

	/*
	 * And close the writing end of the pipe so that read() can return EOF
	 * in the parent.
	 */
	if ((ret = posix_spawn_file_actions_addclose(&factions, p[1])) != 0)
		errx(1, "posix_spawn_file_actions_addclose(): %s", strerror(ret));

	argv[0] = "cal";
	argv[1] = NULL;

	/* Use something that generates several lines of output. */
	ret = posix_spawn(&pid, "/bin/cal", &factions, NULL, argv, NULL);
	if (ret != 0)
		errx(1, "posix_spawn: %d: %s", ret, strerror(ret));

	printf("Proces with PID %ld created.\n", pid);
	while ((n = read(p[0], buf, BUFLEN)) > 0) {
		buf[n] = '\0';
		/*
		 * We use "OUTPUT" string so that we know that we write the
		 * data, not the child.
		 */
		printf("OUTPUT: '%s'\n", buf);
	}

	if (n == -1)
		err(1, "read");

	/*
	 * This doesn not have to be here because we are going to exit. It is
	 * here for reference purpose so that you know that if you are going to
	 * continue in the process, you should destroy that and free the
	 * allocated memory.
	 */
	if ((ret = posix_spawn_file_actions_destroy(&factions)) != 0)
		errx(1, "posix_spawn_file_actions_destroy(): %s", strerror(ret));

	return (0);
}
