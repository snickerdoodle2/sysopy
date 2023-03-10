#include <stdio.h>
#include <stdlib.h>

int fill_buffer(FILE * file, char * buffer){
	return fread(buffer, sizeof(char), 1, file);
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

	FILE * in_file = fopen(in_file_path, "r");
	if (in_file == NULL){
		printf("Nie udalo sie odczytac pliku wejsciowego!\n");
		return 1;
	}

	FILE * out_file = fopen(out_file_path, "w");
	if (out_file == NULL){
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
		fwrite(buffer, sizeof(char), 1, out_file);
	}
	


	fclose(in_file);
	fclose(out_file);
	return 0;
}
