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
 * legacy IPv4-only app
 *
 * Task: convert the app to be AF-agnostic
 *       and preserve the behavior as much as possible
 */

#include <stdio.h>
#include <string.h>
#include <err.h>
#include <netinet/in.h>
#include <netdb.h>

void
get_addr(struct sockaddr_in *dst, char *host, char *port) {
	struct hostent *he;
	struct servent *se;

	dst->sin_addr.s_addr = INADDR_NONE;

	/* Avoid naming lookups so try inet_addr() first. */
	if ((dst->sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		if ((he = gethostbyname(host)) != NULL) { 
			printf("host resolved\n");
			if (he->h_length > sizeof (dst->sin_addr))
				errx(1, "addr len mismatch for host '%s'",
				    host); 
			dst->sin_family = he->h_addrtype; 
  			memcpy(&dst->sin_addr, he->h_addr_list[0],
			    he->h_length);
		}
	}

	if (dst->sin_addr.s_addr == INADDR_NONE)
		errx(1, "failed to get addr of '%s'", host);

	printf("Got address: %s\n", inet_ntoa(dst->sin_addr));

	if ((se = getservbyname(port, "udp")) != 0) {
		dst->sin_port = ntohs(se->s_port);
	} else {
		if ((dst->sin_port = atoi(port)) == 0)
			errx(1, "TCP port lookup failed: %s", port);
	}

	printf("Got port: %d\n", dst->sin_port);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in dst;
	char *host, *port;

	if (argc != 3)
		errx(1, "usage: %s <host|IP> <port>", argv[0]);

	host = argv[1];
	port = argv[2];

	/* Get IP address first */
	get_addr(&dst, host, port);

	/* Connect and write some data */

	/* Print the address of the responder */

	return (0);
}
