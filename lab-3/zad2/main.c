#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
	if (argc < 2) {
		printf("Podaj argument dla ls!\n");
		return 1;
	}

	printf("%d: ", getpid());
	fflush(stdout);
	execl("/bin/ls", "ls", argv[1], NULL);
	return 0;
}