#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "my_lib.h"

#define INPUT_SIZE 256

Table * exec_init(char * argv) {
	return tab_init(atoi(argv));
}

void exec_count(char * argv, Table * table) {
	if (table == NULL) return;
	FILE * file = fopen(argv, "r");
	if (file == NULL) {
		printf("Podany plik nie istnieje!\n");
		return;
	}
	fclose(file);

	count_file("makefile", table);
}

void exec_show(char * argv, Table * table) {
	if (table == NULL) return;
	printf("%s", block_content(atoi(argv), table));
}

void exec_delete(char * argv, Table * table) {
	if (table == NULL) return;
	remove_block(atoi(argv), table);
}

void exec_destroy(Table * table) {
	if (table == NULL) return;
	free_table(table);
}


int main() {
	char input[INPUT_SIZE];
	Table * table = NULL;

	while(fgets(input, INPUT_SIZE, stdin) != NULL) {
		input[strlen(input) - 1] = '\0';


		char * ptr;
		ptr = strtok(input, " ");
		char command[10] = "";
		strcpy(command, ptr);

		ptr = strtok(NULL, " ");
		char argument[128] = "";
		if (ptr != NULL) {
			strcpy(argument, ptr);
		}

		if (strcmp(command, "init") == 0) {
			table = exec_init(argument);
		} else if (strcmp(command, "count") == 0) {
			exec_count(argument, table);
		} else if (strcmp(command, "show") == 0) {
			exec_show(argument, table);
		} else if (strcmp(command, "delete") == 0) {
			exec_delete(argument, table);
		} else if (strcmp(command, "destroy") == 0) {
			exec_destroy(table);
		} else {
			printf("Nieznana komenda!\n");
		}
	
	}

	return 0;
}