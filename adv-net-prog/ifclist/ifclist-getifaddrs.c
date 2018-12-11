/*
 * Get list interfaces which are up. getifaddrs() returns list of addresses
 * rather than interfaces.
 *
 * This works on Linux, BSD.
 *
 * Vladimir Kotal, 2015
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

int
main(int argc, char *argv[])
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1)
		err(1, "getifaddrs() failed");

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL)
			continue;

		if (ifa->ifa_flags & IFF_UP == 0)
			continue;

		printf("%s\n", ifa->ifa_name);
	}

	freeifaddrs(ifaddr);

	return (0);
}
