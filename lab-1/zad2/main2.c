#include <dlfcn.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define INPUT_SIZE 256

typedef struct Table {
	char ** blocks;
	int max_items;
	int cur_items;
} Table;

Table * (*tab_init)(int size);
void (*count_file)(const char * filename, Table * tab);
char * (*block_content)(int index, Table * tab);
void (*remove_block)(int index, Table * tab);
void (*free_table)(Table * tab);




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

	count_file(argv, table);
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

	void *my_lib = dlopen("./libmy_lib.so", RTLD_LAZY);

	tab_init = dlsym(my_lib, "tab_init");
	if(dlerror()!=NULL){printf("blad tab_init\n"); return 1;}
	count_file = dlsym(my_lib, "count_file");
	if(dlerror()!=NULL){printf("blad count_file\n"); return 1;}
	block_content = dlsym(my_lib, "block_content");
	if(dlerror()!=NULL){printf("blad block_content\n"); return 1;}
	remove_block = dlsym(my_lib, "remove_block");
	if(dlerror()!=NULL){printf("blad remove_block\n"); return 1;}
	free_table = dlsym(my_lib, "free_table");
	if(dlerror()!=NULL){printf("blad free_table	\n"); return 1;}


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

	dlclose(my_lib);

	return 0;
}