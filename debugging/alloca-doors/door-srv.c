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
 * example of door server with memory leak. Answers requests of the door
 * client by copying out local configuration.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <door.h>
#include <stropts.h> /* for fattach() */
#include <alloca.h>

#include "config.h"

config_t global_config;

void
process_req(void *cookie, char *argp, size_t arg_size, door_desc_t *dp,
	uint_t n_desc) {
	char *data;

	printf("got request\n");
	/* XXX verify MAGIC_NUMBER */

#ifdef MALLOC
	if ((data = malloc(sizeof (config_t))) == NULL) {
		warnx("malloc");
		return; 
	}
#else
	if ((data = alloca(sizeof (config_t))) == NULL) {
		warnx("alloca");
		return; 
	}
#endif
	memcpy(data, (void *)&global_config, sizeof (config_t));

	door_return(data, sizeof (config_t), NULL, 0);
	/* NOTREACHED */
}

int
main(int argc, char *argv[]) {
	int door_fd, tmp_fd;
	struct stat buf;
	char *door_file;

	if (argc != 2)
		errx(1, "usage: %s <door_file>", argv[0]);

	/* initialize global configuration */
	global_config.instances = 13;
	(void) strlcpy(global_config.config_file, argv[1],
	    sizeof (global_config.config_file));

	door_file = argv[1];
	if ((door_fd = door_create(process_req, 0, 0)) < 0)
		err(1, "door create");

	/* make sure file system location exists */
	if (stat(door_file, &buf) < 0) {
		/* create the entry */
		if ((tmp_fd = creat(door_file, 0400)) < 0)
			err(1, "creat");
		close(tmp_fd);
	}

	/* make sure nothing else is attached */
	if ((fdetach(door_file) < 0) && (errno != EINVAL))
		err(1, "fdetach");

	/* attach to file system */
	if (fattach(door_fd, door_file) < 0)
		err(1, "fattach");

	/* wait for requests */
	while (pause())
		;

	close(door_fd);

	return (0);
}
