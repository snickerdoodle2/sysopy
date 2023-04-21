#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

double function(double x) {
	double mianownik = x*x + 1;
	return 4.0 / mianownik;
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

int main(int argc, char ** argv) {
	if (argc < 3) return 0;
	double start = atof(argv[0]);
	double stop = atof(argv[1]);
	double dx = atof(argv[2]);
	int stream = open("potok", O_WRONLY);
	if (stream == -1) return 0;
	double result = integrate(function, start, stop, dx);
	write(stream, &result, sizeof(double));
	close(stream);

	return 0;
}
