#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int fill_buffer(int file, char * buffer){
	return read(file, buffer, sizeof(char));
}

int main(int argc, char *argv[]) {
	if (argc < 5) {
		printf("Za mało argumentów!\n");
		return 1;
	}

	char char_find = argv[1][0];
	char char_replace[2] = {argv[2][0], '\0'};
	char * in_file_path = argv[3];
	char * out_file_path = argv[4];

	int in_file = open(in_file_path, O_RDONLY);
	if (in_file == -1){
		printf("Nie udalo sie odczytac pliku wejsciowego!\n");
		return 1;
	}


	int out_file = open(out_file_path, O_WRONLY | O_CREAT, 0666 ); 
	if (out_file == -1){
		printf("Nie udalo sie odczytac pliku wyjsciowego!\n");
		return 1;
	}

	char buffer[1];
	int ok;

	while(1){
		ok = fill_buffer(in_file, buffer);
		if (!ok) break;
		if (buffer[0] == char_find){
			buffer[0] = char_replace[0];
		}
		write(out_file, buffer, sizeof(char));
	}
	


	close(in_file);
	close(out_file);
	return 0;
}
