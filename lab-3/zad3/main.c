#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <sys/wait.h>

int find_pattern(const char * filename, const char * pattern) {
	FILE * file = fopen(filename, "r");
	if (file == NULL) return 0;

	int res;
	res = fseek(file, 0, SEEK_END);
	if (res != 0) return 0;

	int file_length = ftell(file);
	int pattern_length = strlen(pattern);

	res = fseek(file, 0, SEEK_SET);
	if (res != 0) return 0;

	char * file_buffer = calloc(pattern_length, sizeof(char));
	while (fread(file_buffer, sizeof(char), pattern_length, file)) {
		if (strcmp(file_buffer, pattern) == 0) return 1;

		if (ftell(file) == file_length) break;
		res = fseek(file, -(pattern_length-1), SEEK_CUR);
		if (res != 0) return 0;
	}



	free(file_buffer);
	fclose(file);

	return 0;
}

void rec_read_dir(const char * start_dir, const char* pattern) {
	DIR * directory = opendir(start_dir);

	if (directory == NULL) return;

	struct dirent * entry;
	struct stat stats_buf;

	char path_buffer[PATH_MAX];

	while ((entry = readdir(directory)) != NULL) {
		// SKIP . AND .. DIRECTORIES
		if (strcmp(entry->d_name, ".") == 0 
		|| strcmp(entry->d_name, "..") == 0) continue;

		sprintf(path_buffer, "%s/%s", start_dir, entry->d_name);

		int stat_ok = stat(path_buffer, &stats_buf);
		if (stat_ok == -1) continue;

		if (S_ISDIR(stats_buf.st_mode)) {
			pid_t pid = fork();
			if (pid == 0) {
				execl("./main", "main", path_buffer, pattern, NULL);
			}
			else {
				int status = 0;
				wait(&status);
			}
		} else {
			if(find_pattern(path_buffer, pattern)) 
				printf("%s %d\n", path_buffer, getpid());
		}
	}

	closedir(directory);
}

int main(int argc, char * argv[]) {
	if (argc < 3) {
		printf("Za malo argumentow!\n");
		return 1;
	}

	const char * start_dir = argv[1];
	const char * pattern = argv[2];

	if (strlen(pattern) > 255) {
		printf("Pattern nie moze miec wiecej niz 255 znakow!\n");
		return 1;
	}

	rec_read_dir(start_dir, pattern);

	return 0;
}
