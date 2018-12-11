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
 * For demonstrating the limitations of libumem.so and watchmalloc.so
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	char *buffer;
	int i, offset, count, len;
	char pattern = 0;

	if (argc < 4) {
		errx(1, "usage: %s <buf_len> <offset> <count> [pattern]"
		    "\nuse pattern other than 'A'",
		    argv[0]);
	}

	len = atoi(argv[1]);
	offset = atoi(argv[2]);
	count = atoi(argv[3]);
	printf("Offset = %d, count = %d\n", offset, count);

	if (argc == 5) {
		pattern = (char)argv[4][0];
		printf("Will use overwrite pattern: '%c' (0x%x)\n",
		    pattern, pattern);
	}

	/* Fill the buffer with A's. */
	if ((buffer = malloc(len)) == NULL)
		err(1, "malloc");
	memset(buffer, 'A', len);

	printf("Buffer address = 0x%p\n", buffer);

	for (i = offset; i < (int)(offset + count); i++) {
		if (pattern != 0)
			*(buffer + i) = pattern;
		printf("0x%p: 0x%x\n", buffer + i, *(buffer + i));
	}

	return (0);
}
