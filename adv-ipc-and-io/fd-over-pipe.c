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
 * Send a file descriptor over a pipe. The child opens the file, sends the file
 * descriptor, and the parent uses it to read the file. You can do it the other
 * way around, of course.
 */

#include <sys/types.h>
#include <stropts.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>

#define	BUFMAX		10
#define	FILENAME	"/etc/passwd"

int
main(void)
{
	int p[2];

	if (pipe(p) == -1)
		err(1, "pipe");

	/* child */
	if (fork() == 0) {
		int fd;

		close(p[0]);
		/* fd should be 3 */
		if ((fd = open(FILENAME, O_RDONLY)) == -1)
			err(1, "open");

		printf("fd number for %s is %d\n", FILENAME, fd);
		if (ioctl(p[1], I_SENDFD, fd) == -1)
			err(1, "ioctl in the child");
		close(fd);
	} else {
		char buf[BUFMAX + 1];
		struct strrecvfd getfd;

		/*
		 * Skip this to show that the sent fd number can be different
		 * between processes.
		 */
		/* close(p[1]); */

		/*
		 * Make sure the parent closed the fd so that we can see that we
		 * have our own file descriptor now, indepedent on the father's
		 * one.
		 */
		sleep(1);
		if (ioctl(p[0], I_RECVFD, &getfd) == -1)
			err(1, "ioctl in the parent");

		printf("received fd number is %d\n", getfd.fd);
		printf("uid/gid of the sending process %d/%d\n", getfd.uid,
		    getfd.gid);
		printf("will read just %d bytes from %s.\n", BUFMAX, FILENAME);
		read(getfd.fd, buf, BUFMAX);
		buf[BUFMAX] = '\0';
		printf("read data from the file: '%s'\n", buf);
	}

	return (0);
}
