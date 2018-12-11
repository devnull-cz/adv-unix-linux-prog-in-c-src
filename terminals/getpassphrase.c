/*
 * Run like this:
 *
 *	$ ./a.out </dev/null
 *
 * to see that it will use the terminal anyway no matter what you do with the
 * standard input. Then, get rid of the terminal, eg. like this:
 *
 *	$ ssh -T localhost
 *	./a.out
 *	a.out: (6) getpassphrase: No such device or address
 *
 * And in the man for getpassphrase(3C) you can see:
 *
 *	ENXIO     The process does not have a controlling terminal.
 */
#include <stdlib.h>
#include <err.h>
#include <errno.h>

int
main(void)
{
	char *pass;

	if ((pass = getpassphrase("Enter PIN: ")) == NULL)
		err(1, "(%d) getpassphrase", errno);

	printf("You entered: '%s'\n", pass);

	return (0);
}
