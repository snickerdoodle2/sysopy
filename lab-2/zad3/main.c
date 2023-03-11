#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

int main() {
	DIR * directory = opendir("./");
	if (directory == NULL) {
		fprintf( stderr, "Wystapil problem z otwarciem katalogu!\n" );
		return 1;
	}

	struct dirent * entry;
	struct stat stats_buf;

	long long size_sum = 0;

	while ((entry = readdir(directory)) != NULL) {
		int ok = stat(entry->d_name, &stats_buf);
		if (ok == -1) {
			fprintf( stderr, "Wystapil problem z zczytaniem pliku!\n" );
			return 1;
		}

		if (S_ISDIR(stats_buf.st_mode)) continue;
		printf("%ld %s\n", stats_buf.st_size ,entry->d_name);

		size_sum += stats_buf.st_size;

	}

	printf("%lld total\n", size_sum);

	closedir(directory);
	return 0;
}
