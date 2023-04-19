#include <stdio.h>
#include <stdlib.h>

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
	double rect_width = atof(argv[1]);
	int n = atoi(argv[2]);

	int * amount_per_child = calloc(n, sizeof(int));

	double start = 0.0;
	while (start < 1){
		for (int i = 0; i < n; i++) {
			amount_per_child[i]++;
			start += rect_width;
		}
	}

	for (int i = 0; i < n; i++) {
		printf("%d\n", amount_per_child[i]);
	}

	return 0;
}
