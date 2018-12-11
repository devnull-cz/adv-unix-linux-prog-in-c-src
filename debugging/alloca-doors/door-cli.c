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
 * Example of door client. Sends a query to door server in order to get
 * its configuration in the form of config_t structure.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <door.h>

#include "config.h"

void
print_config(config_t *config) {
	if (config == NULL)
		errx(1, "NULL config");

	printf("config:\n\tnum = %d\n\tstr = \"%s\"\n",
	    config->instances,
	    config->config_file);
}

int
main(int argc, char *argv[]) {
	char *door_file;
	int door_fd;
	door_arg_t arg;
	int req = MAGIC_NUMBER;

	if (argc != 2)
		errx(1, "usage: %s <door_file>", argv[0]);

	door_file = argv[1];

	if ((door_fd = open(door_file, O_RDONLY)) < 0)
		err(1, "open door");

	arg.data_ptr = (char *)&req;
	arg.data_size = sizeof (int);
	arg.desc_ptr = NULL;
	arg.desc_num = 0;
	arg.rbuf = (char *)NULL;
	arg.rsize = 0;

	if (door_call(door_fd, &arg) < 0)
		err(1, "door call");

	print_config((config_t *)(arg.rbuf));

	return (0);
}
