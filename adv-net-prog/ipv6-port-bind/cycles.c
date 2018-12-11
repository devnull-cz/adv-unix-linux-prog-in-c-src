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
 * Demonstrate the impact of CVE-2008-1483.
 *
 * The idea comes from
 * onnv-gate/usr/src/cmd/ssh/libssh/common/channels.c:x11_create_display_inet()
 */

#include <stdio.h>
#include <err.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define PORT_MAX	5

int main(int argc, char *argv[]) {
	struct addrinfo hints, *ai, *res0;
	uint16_t port, init_port;
	char strport[NI_MAXSERV];
	int sock;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	if (argc != 2)
		errx(1, "usage: %s <port>", argv[0]);

	init_port = atoi(argv[1]);

	/* init_port is in the 6k port range for X11 protocol */
	for (port = init_port; port < init_port + PORT_MAX; port++) {
		printf("=== trying port %d\n", port);
		snprintf(strport, sizeof strport, "%d", port);
		if (getaddrinfo(NULL, strport, &hints, &res0) != 0)
			errx(1, "getaddrinfo failed");
		for (ai = res0; ai; ai = ai->ai_next) {
			/* skip invalid address families */
			if (ai->ai_family != AF_INET &&
			    ai->ai_family != AF_INET6)
				continue;
			printf("creating socket for af = %d\n", ai->ai_family);
			sock = socket(ai->ai_family, SOCK_STREAM, 0);
			if (sock < 0) {
				if ((errno != EINVAL) &&
				    (errno != EAFNOSUPPORT))
					errx(1, "socket");
				else {
					warnx("socket failed for port %d," \
					    " continue", port);
					continue;
				}
			}
			if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
				warnx("bind port %d failed", port);
				
				/* try same port and next family */
				if (ai->ai_next != NULL) {
					warnx("trying same port, next family");
					continue;
				}
	
				/* try next port */
				break;
			}
			warnx("got one");
			break;
		} /* inner/family cycle */
	} /* outer/portnum cycle */

	return(0);
}
