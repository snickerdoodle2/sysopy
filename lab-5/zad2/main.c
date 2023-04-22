#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

char * get_time(struct tms cpu_start, struct tms cpu_end, clock_t r_start, clock_t r_end) {
	double elapsed_real = (double) (r_end - r_start) / sysconf(_SC_CLK_TCK);
	double elapsed_user = (double) (cpu_end.tms_utime - cpu_start.tms_utime) / sysconf(_SC_CLK_TCK);
	double elapsed_system = (double) (cpu_end.tms_stime - cpu_start.tms_stime) / sysconf(_SC_CLK_TCK);

	char * out = calloc(256, sizeof(char));

	sprintf(out, "real: %f user: %f system: %f\n", elapsed_real, elapsed_user, elapsed_system);

	return out;
}

double integrate(double f(double), double start, double stop, double dx) {
	double out = 0;
	double x = start;
	while (x < stop) {
		out += f(x) * dx;
		x += dx;
	}
	return out;
}

double function(double x) {
	double mianownik = x*x + 1;
	return 4.0 / mianownik;
}

int main(int argc, char ** argv) {
	if (argc < 3) {
		printf("Za malo argumentow!\n");
		return 1;
	}
	FILE * result_file = fopen("results.txt", "w");
	if (result_file == NULL) {
		printf("Nie udalo sie otworzyc pliku!");
		return 1;
	}

	double rect_width = atof(argv[1]);
	int n = atoi(argv[2]);

	int * amount_per_child = calloc(n, sizeof(int));
	int * read_pipes = calloc(n, sizeof(int));

	double start = 0.0;
	while (start < 1){
		for (int i = 0; i < n; i++) {
			amount_per_child[i]++;
			start += rect_width;
			if (start >= 1) break;
		}
	}

	struct tms cpu_start, cpu_end;
	clock_t r_start = times(&cpu_start);

	double stop = 0.0;
	int child_pid;
	for (int i = 0; i < n; i++) {
		int fd[2];
		pipe(fd);

		start = stop;
		stop = start + amount_per_child[i] * rect_width;
		if (stop > 1) stop = 1;

		child_pid = fork();

		if (child_pid == 0) {
			close(fd[0]);
			double pi_part = integrate(function, start, stop, rect_width);
			write(fd[1], &pi_part, sizeof(double));
			break;
		} else {
			close(fd[1]);
			read_pipes[i] = fd[0];
		}
	}

	if (child_pid == 0) return 0;

	double pi = 0.0;

	for (int i = 0; i < n; i++) {
		double pi_part;
		read(read_pipes[i], &pi_part, sizeof(double));
		pi += pi_part;
	}

	clock_t r_end = times(&cpu_end);
	char * time_elapsed = get_time(cpu_start, cpu_end, r_start, r_end);
	fwrite(time_elapsed, 1, strlen(time_elapsed), result_file);
	fclose(result_file);
	

	printf("%.30lf\n", pi);

	free(amount_per_child);
	free(read_pipes);

	return 0;
}
