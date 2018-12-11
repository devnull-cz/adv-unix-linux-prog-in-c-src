#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void
handler(int sig)
{
	printf("SIGTTIN caught...\n");
}

int main(void)
{
	char c;
	struct sigaction act;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGTTIN, &act, NULL);
	printf("trying to read from the terminal...\n");
	read(0, &c, 1);

	return (0);
}
