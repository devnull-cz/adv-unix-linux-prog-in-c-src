/*
 * Get list of interfaces and their indices using if_nameindex() function.
 * This is POSIX interface.
 *
 * Vladimir Kotal, 2015
 */

#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <net/if.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	struct if_nameindex *if_list, *ifp;

	if ((if_list = if_nameindex()) == NULL)
		err(1, "cannot get list of interfaces\n");

	/*
	 * if_nameindex() returns array of if_nameindex structures,
	 * terminated by null structure.
	 */
	for (ifp = if_list; ifp->if_index != 0 && ifp->if_name != NULL; ifp++)
		printf("%u: %s\n", ifp->if_index, ifp->if_name);

	/*
	 * TODO: how to get interface status (up, down) and list of addresses
	 * using interface name/index ?
	 */

	if_freenameindex(if_list);

	return (0);
}
