/*
 * Very simple example of how asynchronous I/O can be used.
 */

#include <err.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifdef _POSIX_ASYNCHRONOUS_IO
#include <aio.h>
#else
#error	no asynchronous I/O
#endif

#ifndef _POSIX_REALTIME_SIGNALS
#error	need POSIX real-time signals
#endif

#define	BUF_LEN	512

struct aiocb a;
char *file;

void
aio_handler(int signo, siginfo_t *info, void *ignored)
{
	struct aiocb *ptr = (struct aiocb *)info->si_value.sival_ptr;

	printf("Signal caught.\n");

	if (signo != SIGUSR1 && info->si_code != SI_NOINFO) {
		printf("Caught signal %d.\n", signo);
		return;
	}

	if (aio_error(ptr) != 0) {
		if (aio_error(ptr) != EINPROGRESS)
			_exit(1);
		else
			return;
	}

	/* Print out what we read. */
	printf("=== First %d bytes of %s ===\n", BUF_LEN, file);
	write(1, (const void *)ptr->aio_buf, aio_return(ptr));
	write(1, "\n", 1);

	/* Finished. */
	_exit(0);
}

int
main(int argc, char **argv)
{
	int fd;
	char buf[BUF_LEN];
	struct sigaction act;

	if (argc != 2)
		errx(1, "file expected as argv[1]");

	if ((fd = open(argv[1], O_RDONLY)) == -1)
		err(1, "open");

	/* Be explicit. */
	memset(&a, 0, sizeof (a));
	file = argv[1];

	a.aio_fildes = fd;
	a.aio_buf = buf;
	a.aio_nbytes = BUF_LEN;
	a.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	a.aio_sigevent.sigev_signo = SIGUSR1;
	/* Address of the aiocb structure. */
	a.aio_sigevent.sigev_value.sival_ptr = (void *)&a;

        memset(&act, 0, sizeof(act));
        act.sa_sigaction = aio_handler;
        act.sa_flags = SA_SIGINFO;
        (void) sigaction(SIGUSR1, &act, NULL);

	if (aio_read(&a) == -1)
		err(1, "aio_read");

	printf("Waiting for completion now...\n");

	while (1)
		pause();
	
	/* Not reached. */
	return (0);
}
