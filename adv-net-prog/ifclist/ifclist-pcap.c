/*
 * get list of interfaces which are up using libpcap.
 *
 * Vladimir Kotal, 2015
 */

#include <stdio.h>
#include <string.h>
#include <err.h>
#include <pcap/pcap.h>

int
main(int argc, char *argv[])
{
        char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *alldevsp, *dev;

	if (pcap_findalldevs(&alldevsp, errbuf) == -1)
		errx(1, "cannot find devices: %s\n", errbuf);

	for (dev = alldevsp; dev->next != NULL; dev = dev->next) {
		/* Print only interfaces which are up. */
		if (dev->flags & PCAP_IF_UP) {
			/* "any" interface is special in pcap */
			if (!strcmp("any", dev->name))
				continue;

			printf("%s\n", dev->name);
		}
	}

	pcap_freealldevs(alldevsp);

	return (0);
}
