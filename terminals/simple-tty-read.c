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
 * The same as simple-read.c but now we explicitly open /dev/tty. The result is
 * the same (unless we redirect input/output because in that case, this program
 * will still read the data from the terminal, thus ignoring any redirection).
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>

int
main(void)
{
	int fd, n;
	char buf[1024];

	if ((fd = open("/dev/tty", O_RDWR)) == -1)
		err(1, "(%d) open", errno);

	printf("will read/write from/to /dev/tty...\n");
	while (1) {
		printf("YOUR INPUT: ");
		/*
		 * printf() above didn't use '\n' so we must tell stdio to flush
		 * the output.
		 */
		fflush(stdout);
		if ((n = read(fd, buf, sizeof(buf) - 1)) == -1)
			err(1, "read");

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
