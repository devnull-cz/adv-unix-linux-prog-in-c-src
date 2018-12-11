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
 * This program contain 2 types of resource leaks. Find them and fix them.
 */

#include <stdio.h>
#include <err.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

char *config_file = NULL;

typedef struct entry_s {
	int n;
	char *s;
	struct entry_s *next;
} entry_t;

entry_t *entries = NULL;

/*
 * Load configuration file into memory. Return -1 on error, 0 on success.
 */
int
load_config(void) {
	FILE *fp;
	char buf[255];
	int i = 0;
	entry_t *e;

	if ((fp = fopen(config_file, "r")) == NULL) {
		warnx("cannot open file");
		return (-1);
	}

	printf("Loading config file from scratch\n");

	/*
	 * This might look stupid but parsing config files is usually
	 * much more complicated, with several levels of hierarchy which
	 * leave room for forgetting free(), especially when handling
	 * errors for higher-level grammar constructs.
	 */
	while (fgets(buf, sizeof (buf), fp) != NULL) {
		e = malloc(sizeof (entry_t));
		e->n = i++;
		e->s = strdup(buf);
		e->next = entries;
		entries = e;
	}

	return (0);
}

/*
 * SIGHUP handler, reloads configuration
 */
void
reload(int s) {
	entry_t *e = entries;
	entry_t *tmp;

	printf("SIGHUP received, reloading config\n");
	/* destroy the list of entries */
	while (e != NULL) {
		tmp = e;
		e = e->next;
		free(tmp);
	}
	entries = NULL;

	/* refresh the config */
	(void) load_config();
}

int
main(int argc, char *argv[]) {
	struct sigaction sa;

	if (argc != 2)
		errx(1, "usage: %s <config_file>", argv[0]);

	config_file = argv[1];

	if (load_config() < 0)
		errx(1, "failed to load config file '%s'", config_file);

	/* set signal handler */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;		/* don't restart system calls */
	sa.sa_handler = reload;
	(void) sigaction(SIGHUP, &sa, NULL);

	/* wait for signals, daemonize, whatever */
	while (pause())
		;
}
