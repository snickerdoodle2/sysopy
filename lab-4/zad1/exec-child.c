#include <stdio.h>
#include <signal.h>
#include <string.h>


int main(int argc, char ** argv) {
	sigset_t set;

	if (argc < 2) {
		printf("Więcej argumentów!\n");
		return 1;
	}

	char * command = argv[1];

 	if (strcmp(command, "pending") == 0) {
		sigpending(&set);
	    if (sigismember(&set, SIGUSR1) == 1) {
	    	printf("SIGUSR1 czeka w masce (dziecko)!\n");
	    }
  	} else raise(SIGUSR1);

	return 0;
}