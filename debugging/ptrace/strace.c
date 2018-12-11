/*
 * Skeleton of strace program.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>

int
main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	char *filename;
	int i;

	if (argc == 1)
		errx(1, "usage: %s <program> [arg1 .. argn]", argv[0]);
	argv++;
	argc--;
	filename = argv[0];

	switch (pid = fork()) {
		case -1:
			err(1, "fork");
			break;
		case 0:
			printf("execve\n");
			fflush(stdout);
			execve(filename, argv, NULL);
			break;
		default:

			printf("created child with pid %d\n", pid);
			waitpid(pid, &status, 0);
			break;
	}

	return (0);
}
