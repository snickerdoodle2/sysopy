#include <stdio.h>
#include <ftw.h>

long long size_sum = 0;

int handle_entry(const char *fpath, const struct stat *sb, int typeflag){
	if (typeflag == FTW_D) return 0;
	printf("%s %ld\n", fpath, sb->st_size);
	size_sum += sb->st_size;
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Podaj katalog!\n");
		return 1;
	}

	int res = ftw(argv[1], &handle_entry, 1);

	if (res == -1) {
		fprintf(stderr, "Wystapil blad!\n");
		return 1;
	}

	printf("total %lld\n", size_sum);

	return 0; 
}