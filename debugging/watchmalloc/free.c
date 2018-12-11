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
 * Various bad things to do with free
 */

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

int
main(int argc, char *argv[]) {
	char *buffer;
	unsigned int len;
	int i;

	if (argc != 2)
		errx(1, "usage: %s <buf_len>", argv[0]);

	len = atoi(argv[1]);

	if ((buffer = malloc(len)) == NULL)
		err(1, "malloc");

	/* fill the buffer with pattern */
	memset(buffer, 'A', len);

	/* print the buffer contents */
	printf("Buffer address = 0x%p\n", buffer);
	for (i = 0; i < len; i++)
		printf("0x%p: 0x%x\n", buffer + i, *(buffer + i));

	/* double free */
	printf("First free()\n");
	free(buffer);
	printf("Second free()\n");
	free(buffer);

	/* XXX add more */

	return (0);
}
