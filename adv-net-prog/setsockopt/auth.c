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

/* Setup per-socket IPsec policy providing traffic authentication for UDP */

#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
	struct ipsec_req req;
	/* sa_family_t family; */
	int family;
	int sock;
	int port;
	int bypass;
	struct in_addr addr;
	struct in6_addr addr6;
	
	/* let's use IPv4 for now. we can make it an option later. */
	family = AF_INET;

	if (argc != 4)
		errx(1, "usage: %s <0|1> <address> <port_num>", argv[0]);

	bypass = atoi(argv[1]);

	/* XXX */
	if (inet_pton(family, argv[2], addr) < 0)
		errx(1, "inet_pton");
	port = atoi(argv[3]);

	if ((sock = socket(family, SOCK_DGRAM, 0)) < 0)
		err(1, "socket");

	/* fill the bypass request - no ESP or AH */
	(void) memset(&req, 0, sizeof (req));
	req.ipsr_ah_req = XXX
	req.ipsr_esp_req = IPSEC_PREF_NEVER;
	XXX

	if (bypass && (setsockopt(sock, (family == AF_INET) ? IPPROTO_IP :
	    IPPROTO_IPV6, IP_SEC_OPT, &req, sizeof (req)) < 0)) {
		if (errno == EPERM)
			errx(1, "Insufficient permissions");
		else
			err(1, "setsockopt");
	}

	/* we can send and receive now */
	/* XXX */
}
