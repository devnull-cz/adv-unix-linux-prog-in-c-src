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
 * Watch interface events (up/down) via routing socket
 * 
 * This is useful when running server which has to listen separately
 * on all addresses present in the system.
 *
 */

#include <stdio.h>
#include <err.h>
#include <net/if.h>
#include <net/route.h>

#define	BUFLEN	1024

/* handle message from routing socket */
int handle_message(int sock) {
	uint64_t buffer[BUFLEN];
	struct sockaddr_storage *ifa, *ifp;
	struct sockaddr_dl *dl;
	ifa_msghdr_t *ifam = (ifa_msghdr_t *)buffer;
	int nr;

	if ((nr = read(sock, ifam, sizeof (buffer))) < 0)
		err(1, "read");

	if (ifam->ifam_version != RTM_VERSION)
		errx(1, "bad version");

	/* We are only interested in interface additions/removals */
	if ((ifam->ifam_type != RTM_NEWADDR) &&
	    (ifam->ifam_type != RTM_DELADDR))
		return;

	/* Extract interface addresses */
	ifa = 
	RTA_IFA
	ifam 
	nr

	/* List is empty */
	if (ifa == NULL) {
		warnx("no addresses");
		return;
	}

	/* Get interface name */
	ifp =
	RTA_IFP
	ifam
	nr

	dl = (struct sockaddr_dl *)ifp;

	switch (ifam->ifam_type) {
		case RTM_NEWADDR:
			warnx("got NEW");
			/*
			 * Spawn another server instance here listening on
			 * this address.
			 */
			break;
		case RTM_DELADDR:
			warnx("got DEL");
			/*
			 * Delete existing server instance
			 */
			break;
	}
}

int main(void) {
	int rt_sock;

	/* Open routing socket */
	rt_sock = socket(PF_ROUTE, SOCK_RAW, 0);
	if (rt_sock < 0)
		err(1, "socket");

	/* Enter event loop, watching for events */
	select()
		handle_message(rt_sock);

}
