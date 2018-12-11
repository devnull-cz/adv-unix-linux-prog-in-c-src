/*
 * A simple tty(1) program.
 */

#include <fcntl.h>
#include <err.h>
#include <unistd.h>

int main(void)
{
	char *tty_name;

	/* Returns a static string. See ttyname_r(3C) for an MT version. */
	if ((tty_name = ttyname(0)) == NULL)
		err(1, "ttyname");

	printf("%s\n", tty_name);

	return (0);
}
