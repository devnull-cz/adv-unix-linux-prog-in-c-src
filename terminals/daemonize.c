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
 * Copyright 2009, 2010, Jan Pechanec, Vladimir Kotal.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Shows how to create a daemon. The daemon just sleeps for some time,
 * simulating work that way. We will not wait for it to exit. Actually, the
 * daemon() function fork()s twice. The second fork there is to make sure that
 * the non-session leader cannot acquire a controlling terminal by opening any
 * terminal.
 *
 * Default number of seconds is 15, user argv[1] to change that if needed.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	int ret;
	char *secs; 

	secs = "15";

	if (argc > 1)
		secs = argv[1];

	if ((ret = fork()) == -1)
		exit(1);

	if (ret > 0) {
		printf("Starting the daemon. It will sleep for %d seconds.\n",
		    secs);
		exit(0);
	}

	/* Child. */
	printf("I'm the child, soon to be the daemon. PID is %d.\n", getpid());
	if (setsid() == -1)
		exit(1);

	printf("New session created, SID is %d.\n", getsid(0));
	printf("Will close 0/1/2 file descriptors now. Good bye.\n");

	/*
	 * Might be better to redirect /dev/null to/from them. It can read or
	 * write using those descriptors without problems. Note that you never
	 * know what the library you use with the daemon might do down there.
	 */
	close(0);
	close(1);
	close(2);

	execl("/bin/sleep", "sleep", secs, NULL);
	printf("exec() failed.\n");
	return (1);
}
