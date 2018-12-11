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
 * The program opens the stdin and what is read in is just printed out on
 * stdout. This example is to present that the user can edit the line before
 * the Enter key is pressed and the app just gets the final result (because of
 * the terminal canonical mode).
 *
 * However, if you redirect the input from a file, for example:
 *
 *	./a.out </etc/motd
 *
 * ...you will see that "LINE READ" is not printed for ever line of that file
 * but for every filled-up buffer; '\n' has no special function here.
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int
main(void)
{
	int n;
	char buf[1024];

	while (1) {
		printf("YOUR INPUT: ");
		/*
		 * printf() above didn't use '\n' so we must tell stdio to flush
		 * the output.
		 */
		fflush(stdout);
		if ((n = read(STDIN_FILENO, buf, sizeof(buf) - 1)) == -1)
			exit(1);

		if (n == 0) {
			(void) printf("END-OF-FILE\n");
			break;
		}

		/* Replace '\n' with '\0' */
		buf[n - 1] = '\0';
		printf("LINE READ: '%s'\n", buf);
	}

	return (0);
}
