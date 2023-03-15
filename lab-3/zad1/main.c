#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
	if (argc < 2) {
		printf("Podaj argument!\n");
		return 1;
	}

	char * end;
	long children_count = strtol(argv[1], &end, 10);

	pid_t child_pid;

	for (long i = 0; i < children_count; i++) {
		child_pid = fork();
		if (child_pid==0) {
			printf("%d %d\n", (int)getppid(), (int)getpid());
			return 0;
		}
	}

	while(wait(0) >0);
	printf("%s\n", argv[1]);
	return 0;
}