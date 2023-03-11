#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Za mało argumentów!\n");
		return 1;
	}

	char * in_file_path = argv[1];
	char * out_file_path = argv[2];

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
	fseek(in_file, 0, SEEK_END);
    long file_length = ftell(in_file);

	for (long i = file_length-1; i >= 0; i--) {
		fseek(in_file, i, SEEK_SET);
		ok = fread(buffer, sizeof(char), 1, in_file);
		if (!ok) {
			printf("Cos poszlo nie tak!\n");
			return 1;
		}
		ok = fwrite(buffer, sizeof(char), 1, out_file);
		if (!ok) {
			printf("Cos poszlo nie tak!\n");
			return 1;
		}
	}


	fclose(in_file);
	fclose(out_file);
}
