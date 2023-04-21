#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char ** argv) {
	if (argc < 3) {
		printf("Za malo argumentow!\n");
		return 1;
	}

	remove("potok");
	int res = mkfifo("potok", 0666);	
	if (res == -1) {
		printf("Nie udalo sie stworzyc potoku!\n");
		return 1;
	}


	double rect_width = atof(argv[1]);
	int n = atoi(argv[2]);

	int * amount_per_child = calloc(n, sizeof(int));
	double start = 0.0;
	while (start < 1){
		for (int i = 0; i < n; i++) {
			amount_per_child[i]++;
			start += rect_width;
			if (start >= 1) break;
		}
	}

	double stop = 0.0;

	for (int i = 0; i < n; i++) {
		if (amount_per_child[i] == 0) break;
		char start_buff[256];
		char stop_buff[256];
		char dx_buff[256];

		start = stop;
		stop = start + amount_per_child[i] * rect_width;
		if (stop > 1) stop = 1;

		gcvt(start, 256, start_buff);
		gcvt(stop, 256, stop_buff);
		gcvt(rect_width, 256, dx_buff);

		if (fork() == 0) {
			execl("child", start_buff, stop_buff, dx_buff, NULL); 
		}

	}

	int stream = open("potok", O_RDONLY);
	if (stream == -1) {
		printf("Nie udalo sie otworzyc potoku!\n");
		return 0;
	}

	double pi = 0.0;

	for (int i = 0; i < n; i++) {
		double pi_part;
		read(stream, &pi_part, sizeof(double));
		pi += pi_part;
	}

	printf("%.30lf\n", pi);

	return 0;
}
