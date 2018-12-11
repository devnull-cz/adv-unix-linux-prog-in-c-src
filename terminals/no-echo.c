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
 * Switch off the terminal echo. It will read the user input and prints out what
 * was it then. The echo is switched on again before exiting.
 */

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define	MAX_BUF	1024
int
main(void)
{
	int n, fd;
	struct termios t;
	char buf[MAX_BUF];

	printf("will switch off the terminal echo but will print out what "
	    "you type.\n");

	if ((fd = open("/dev/tty", O_RDWR)) == -1)
		exit(1);

	if (tcgetattr(fd, &t) == -1)
		exit(2);

	/* Clear the ECHO flag in the structure. */
	t.c_lflag = t.c_lflag & ~ECHO;

	printf("switching the echo off NOW...\n");
	if (tcsetattr(fd, TCSANOW, &t) == -1)
		exit(3);

	printf("now type something, finish with Enter...\n");
	if ((n = read(fd, buf, MAX_BUF)) == -1)
		exit(4);
	/* Overwrite the ending '\n'. */			
	buf[n - 1] = '\0';
	printf("you typed: '%s'\n", buf);

	/* Set the ECHO flag in the structure back. */
	t.c_lflag = t.c_lflag | ECHO;
	printf("switching the echo back ON now...\n");
	if (tcsetattr(fd, TCSANOW, &t) == -1)
		exit(3);

	printf("...and exiting.\n");

	return (0);
}
