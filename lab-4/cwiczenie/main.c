#include <stdio.h>
#include <signal.h>
#include<unistd.h>

int main() {
	signal(SIGINT, SIG_IGN);
	while (1)
	{
		printf("haha\n");
		sleep(2);
	}
	return 0;
}