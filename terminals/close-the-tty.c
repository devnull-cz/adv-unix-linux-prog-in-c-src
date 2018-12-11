/*
 * This program shows that even when you close the controlling terminal (we
 * assume you run the program "normally" from the terminal) you still have it.
 * The program forks and runs ps(1) and pfiles(1) on the parent process to show that
 * it still has the controlling terminal even while it does not have it open.
 *
 * This means you can never get rid of your controlling terminal unless you
 * create a new session.
 */

#include <unistd.h>
#include <err.h>
#include <sys/wait.h>

int
main(void)
{
	char c;
	int p[2];
	pid_t ppid;

	(void) pipe(p);
	ppid = getpid();

	if (fork() == 0) {
		char buf[128];

		/*
		 * Wait for the parent process to close the descriptors (see
		 * below). We cannot do that before the fork since we would have
		 * no stdout to use to print out the information. We would have
		 * to open /dev/tty again.
		 */
		read(p[0], &c, 1);

		if (fork() == 0) {
			printf("Printing info about our main process after it "
			    "closed the controlling terminal.\n");
			snprintf(buf, 128, "-p %d", ppid);
			execl("/bin/ps", "ps", buf, NULL);
			err(1, "execl");
		}

		/*
		 * Ehm, synchronize the output messages to the terminal so that
		 * it's not a mess.
		 */
		(void) wait(NULL);

		printf("\nPrinting info about open files (should be none).\n");
		snprintf(buf, 128, "%d", ppid);
		/*
		 * No need to fork again, just use this process. We might see
		 * only one item - p[1] unless it's also already closed.
		 */
		execl("/bin/pfiles", "pfiles", buf, NULL);
		err(1, "execl");
	}

	if (close(0) == -1)
		err(1, "close");
	if (close(1) == -1)
		err(1, "close");
	if (close(2) == -1)
		err(1, "close");
	close(p[0]);
	/* Tell the child we are done with our file descriptors. */
	write(p[1], "", 1);
	close(p[1]);

	(void) wait(NULL);
	return (0);
}
