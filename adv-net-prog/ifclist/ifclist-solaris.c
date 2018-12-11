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
 * get list of interfaces in the system
 *
 * NOTE: this example works on Solaris only
 *
 * inspired by usr/src/lib/libinetcfg/common/inetcfg.c:get_plumbed_if_list()
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stropts.h>
#include <sys/sockio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>

#define	LIFC_NOXMIT	0x01

int main(int argc, char *argv[]) {
	struct lifnum lifn;
	struct lifconf lifc;
	struct lifreq *lifrp;
	sa_family_t family = AF_INET;
	int flags = LIFC_NOXMIT;
	char *buf;
	int i, sock, num_ifcs, bufsize;

	if (argc != 2)
		errx(1, "usage: %s <4|6>", argv[0]);

	if (!strcmp(argv[1], "4"))
		family = AF_INET;
	else if (!strcmp(argv[1], "6"))
		family = AF_INET6;
	else
		errx(1, "unknown address family");

	/* It does not matter against which family the socket is opened. */
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		err(1, "socket");

	lifn.lifn_family = family;
	lifn.lifn_flags = flags;
	if (ioctl(sock, SIOCGLIFNUM, (char *)&lifn) < 0)
		err(1, "SIOCGLIFNUM");

	num_ifcs = lifn.lifn_count;
	printf("Found %d interfaces\n", num_ifcs);
	bufsize = num_ifcs * sizeof (struct lifreq);
	if ((buf = malloc(bufsize)) == NULL)
		err(1, "malloc");

	lifc.lifc_family = lifn.lifn_family;
	lifc.lifc_flags = lifn.lifn_flags;
	lifc.lifc_len = bufsize;
	lifc.lifc_buf = buf;
	if (ioctl(sock, SIOCGLIFCONF, (char *)&lifc) < 0)
		err(1, "SIOCGLIFCONF");

	lifrp = lifc.lifc_req;
	printf("List of interfaces:\n");
	for (i = 0; i < num_ifcs; i++, lifrp++) {
		printf("%s\n", lifrp->lifr_name);
	}

	return (0);
}
