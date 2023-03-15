#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strrev(char *str)
{
    if (!str || ! *str)
        return str;

    int i = strlen(str) - 1, j = 0;

    char ch;
    while (i > j)
    {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
}

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

	char buffer[1025];
	int ok;
	fseek(in_file, 0, SEEK_END);
    long file_length = ftell(in_file);
	int no_blocks = file_length / 1024 + 1;

	for (int i = no_blocks - 1; i >= 0; i--) {
		fseek(in_file, i*1024, SEEK_SET);
        ok = fread(buffer, sizeof(char), 1024, in_file);
        if (!ok) {
                printf("Cos poszlo nie tak!\n");
                return 1;
        }
		
		buffer[ok] = 0;

        ok = fwrite(strrev(buffer), sizeof(char), ok, out_file);
        if (!ok) {
                printf("Cos poszlo nie tak!\n");
                return 1;
        }
	}


	fclose(in_file);
	fclose(out_file);
}
