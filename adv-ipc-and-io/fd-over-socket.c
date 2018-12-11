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
 * Send a file descriptor over a UNIX domain socket. The child opens the file,
 * sends the file descriptor, and the parent uses it to read the file. You can
 * do it the other way around, of course.
 *
 * See also fd-over-pipe.c program on how to do that over a pipe on SVR4-based
 * systems. Do remember you must use "-l socket" on Solaris since socket calls
 * are not in libc.
 */

/*
 * You need at least SUS (XPG4v2) for this unless you want to use the old
 * message header; see the header file for more information. Also, see
 * standards(5) on Solaris for more information on standards and how to set the
 * requirements.
 */
#define	_XOPEN_SOURCE		1
#define	_XOPEN_SOURCE_EXTENDED	1

#include <sys/types.h>
#include <sys/socket.h>
#include <stropts.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define	BUFMAX		10
#define	FILENAME	"/etc/passwd"

int
main(void)
{
	int fd, s[2];
	struct msghdr msg;
	struct cmsghdr *cmsg_ptr2, *cmsg_ptr = NULL;
	struct iovec iov[1];

	printf("sizeof (struct cmsghdr) is %d\n", sizeof (struct cmsghdr));

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, s) == -1)
		exit(1);
	printf("socketpair() returned %d/%d\n", s[0], s[1]);

	/* child */
	if (fork() == 0) {
		close(s[0]);
		/* fd should be 3 */
		if ((fd = open(FILENAME, O_RDONLY)) == -1)
			exit(2);

		printf("fd number for %s is %d\n", FILENAME, fd);

		memset(&msg, 0, sizeof (msg));

		/*
		 * Send an empty iovec structure. I believe that according to
		 * the spec, there must be at least one item in the list, even
		 * empty.
		 */
		iov[0].iov_base = NULL;
		iov[0].iov_len = 0;
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;

		/*
		 * Control message structure. CMSG_DATA adds the length of the
		 * file descriptor to the length of the control message itself.
		 */
		cmsg_ptr = (struct cmsghdr *)malloc(CMSG_SPACE(sizeof (fd)));
		/*
		 * Must not use sizeof (*cmsg_ptr) since that's just the header,
		 * not the data that follows after the header.
		 */
		memset(cmsg_ptr, 0, CMSG_SPACE(sizeof (fd)));

		/*
		 * Size of data part of the message.
		 */
		cmsg_ptr->cmsg_len = CMSG_LEN(sizeof (fd));
		cmsg_ptr->cmsg_level = SOL_SOCKET;
		/*
		 * This is to say it will be a file descriptor (the "right to
		 * read a file").
		 */
		cmsg_ptr->cmsg_type = SCM_RIGHTS;
		/*
		 * CMSG_DATA returns a pointer to where the data can be stored.
		 * If you see the header file you will see it's right after the
		 * control message header.
		 */
		*((int *)CMSG_DATA(cmsg_ptr)) = fd;

		/* CMSG_FIRSTHDR */

		msg.msg_control = cmsg_ptr;
		msg.msg_controllen = CMSG_SPACE(sizeof (fd));
		/* 0 means OK */
		msg.msg_flags = 0;

		if (sendmsg(s[1], &msg, 0) == -1) {
			perror("child: sendmsg");
			exit(3);
		}
		close(fd);
	} else {
		char buf[BUFMAX + 1];

		/*
		 * Skip this to show that the sent fd number can be different
		 * between processes.
		 */
#if 0
		close(s[1]); */
#endif

		/* hack, socketpair() uses 4/5, not 3. Why? */
		dup2(0, 3);

		/*
		 * Make sure the parent closed the fd so that we can see that we
		 * have our own file descriptor now, indepedent on the father's
		 * one.
		 */
		sleep(1);

		memset (&msg, 0, sizeof (msg));
		iov[0].iov_base = NULL;
		iov[0].iov_len = 0;
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		cmsg_ptr = (struct cmsghdr *)malloc(CMSG_SPACE(sizeof (int)));
		memset(cmsg_ptr, 0, CMSG_SPACE(sizeof (int)));

		msg.msg_control = cmsg_ptr;
		msg.msg_controllen = CMSG_SPACE(sizeof (int));
		/* 0 means OK */
		msg.msg_flags = 0;

		if (recvmsg(s[0], &msg, 0) == -1) {
			perror("father: recvmsg");
			exit(4);
		}

		printf("received fd number is %d\n",
		    *((int *)CMSG_DATA(msg.msg_control)));
		printf("will read just %d bytes from %s.\n", BUFMAX, FILENAME);
		if (read(*((int *)CMSG_DATA(msg.msg_control)), buf, BUFMAX)
		    == -1) {
			perror("read");
			exit(6);
		}

		buf[BUFMAX] = '\0';
		printf("read data from the file: '%s'\n", buf);
	}

	return (0);
}
