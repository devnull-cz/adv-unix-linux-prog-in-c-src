/*
 * Set TERM to xterm and then run the program. It will move the cursor to the
 * position [30, 30]. Note that after this is done, the shell will issue a
 * prompt so the cursor's position will not be absolute [30, 30]. There will be
 * 30 spaces, the prompt, and the cursor. It should be on the 30th line though.
 * The 'cup' capability, which is the abolute cursor movement, is this:
 *
 *	cup=\E[%i%p1%d;%p2%dH
 *
 * See "termcap & terminfo" book for more information.
 */ 
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	char c[10];

	if (strcmp(getenv("TERM"), "xterm") != 0)
		errx(1, "You must use the 'xterm' terminal type. Set TERM "
		    "apropriately.");

	c[0] = 27;
	c[1] = '[';
	c[2] = '3';
	c[3] = '0';
	c[4] = ';';
	c[5] = '3';
	c[6] = '0';
	c[7] = 'H';

	if (write(1, c, 8) != 8)
		err(1, "write");

	return (0);
}
