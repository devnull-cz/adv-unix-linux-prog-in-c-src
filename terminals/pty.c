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
 * Pseudo terminal usage example. The program allocates a master pseudo
 * terminal and forks a child which uses the slave counterpart. The child
 * creates a new session, opens the slave device, making it its
 * controlling terminal, and start a command. Its output is read by the parent
 * from the master terminal, and is printed on stdout.
 *
 * This might work on Solaris only. See those ioctl()'s below, it might be
 * enough to remove those to make it work on other systems.
 */

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stropts.h>
#include <libgen.h>

#define	MAX_BUF	1024

int
main(int argc, char **argv)
{
	int setup, n, ptm, pts;
	char buf[MAX_BUF + 1];	/* +1 for '\0' */

	if (argc != 2) {
		fprintf(stderr, "usage: %s command\n", basename(argv[0]));
		exit(1);
	}

	/*
	 * Note that we don't want this pseudo terminal to become our new
	 * controlling terminal.
	 */
	ptm = posix_openpt(O_RDWR | O_NOCTTY);
	printf("my PID is %d\n", (int)getpid());
	printf("pts name is '%s'\n", ptsname(ptm));

	if (grantpt(ptm) == -1)
		exit(2);

	if (unlockpt(ptm) == -1)
		exit(3);

	switch (fork()) {
	case -1:
		exit(4);
	/* child */
	case  0:
		if (setsid() == -1)
			exit(12);

		/* Make the slave our new controlling terminal. */
		if ((pts = open(ptsname(ptm), O_RDWR)) == -1)
			exit(5);

		/* Check if we need to push ptem and ldterm. */
		if ((setup = ioctl(pts, I_FIND, "ldterm")) < 0)
			exit(6);

		/* If we don't have a terminal semantics yet, do it now. */
		if (setup == 0) {
			printf("CHILD: no ldterm, will push some modules onto "
			    "the stream\n");
			if (ioctl(pts, I_PUSH, "ptem") < 0)
			    exit(7);
			if (ioctl(pts, I_PUSH, "ldterm") < 0)
			    exit(8);
		}

		/* Redirect 0/1/2 to the slave pseudo terminal device. */
		dup2(pts, 0);
		dup2(pts, 1);
		dup2(pts, 2);

		/* Check that stdout is a real terminal now. */
		if (isatty(STDOUT_FILENO) == 1)
			printf("CHILD: descriptor 1 is a terminal\n");
		else {
			printf("CHILD: descriptor 1 is NOT a terminal\n");
			exit(9);
		}

		close(pts);
		close(ptm);

		/*
		 * Now start the command. You should try with someting that
		 * generates data but does not read any.
		 */
		printf("CHILD: starting the command...\n");
		execlp(argv[1], "xxx", NULL);
		printf("could not run %s\n", argv[1]);
		exit(10);
		break;

	/* parent */
	default:
		/*
		 * Read from the master pseudo terminal device, and print all
		 * such data to stdout.
		 */
		while ((n = read(ptm, buf, MAX_BUF)) > 0) {
			buf[n] = '\0';
			printf("%s", buf);
			fflush(stdout);
		}

		if (n == -1)
			exit(11);

		break;
	}

	return (0);
}
