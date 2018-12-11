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
 * flush IPsec AH SAs
 *
 * similar to
 * onnv-gate/usr/src/cmd/cmd-inet/usr.sbin/ipsecutils/ipseckey.c:doflush()
 */

#include <stdio.h>
#include <strings.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <net/pfkeyv2.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(void) {
	struct sadb_msg msg;
	int sock;
	pid_t mypid = getpid();
	
	/* open PF_KEY socket */
	if ((sock = socket(PF_KEY, SOCK_RAW, PF_KEY_V2)) < 0)
		err(1, "socket");

	/* Fill the PF_KEY message structure */
	bzero(&msg, sizeof (msg));
	msg.sadb_msg_version = PF_KEY_V2;
	msg.sadb_msg_type = SADB_FLUSH;
	msg.sadb_msg_errno = 0;
	/* Want to flush AH SAs only */
	msg.sadb_msg_satype = SADB_SATYPE_AH;
	msg.sadb_msg_len = SADB_8TO64(sizeof (msg));
	msg.sadb_msg_reserved = 0;
	/* We will only send 1 message, hence sequence number is 0 */
	msg.sadb_msg_seq = 0;
	msg.sadb_msg_pid = mypid;
	
	/* write the message to the socket */
	if (write(sock, &msg, sizeof (msg)) < 0)
		err(1, "write");

	/* read the response */
	while (read(sock, &msg, sizeof (msg)) > 0) {
		if ((msg.sadb_msg_seq == 0) &&
		    (msg.sadb_msg_pid == mypid))
			break;
	}

	/* Check the response */
	/* NOTE: it should have the same sadb_msg_type and sadb_msg_satype
		 as the request */
	if (msg.sadb_msg_errno != 0) {
		/* force the errno so err() can interpret it */
		errno = msg.sadb_msg_errno;
		err(1, "flush failed");
	}

	printf("AH SAs flushed\n");
}
