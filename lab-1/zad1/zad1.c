#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zad1.h"

typedef struct Table {
	char ** blocks;
	int max_items;
	int cur_items;
} Table;

Table tab_init(int size) {
	Table tab;

	tab.cur_items = 0;
	tab.max_items = size;
	tab.blocks = calloc(size, sizeof(char *));

	return tab;
}

void count_file(const char * filename, Table * tab) {

	// Creating and executing wc command.


	// chyba tyle miejsca trzeba
	// wc <file_name> > /tmp/file_len
	// <file_name> + wc + 3*' ' + > + /tmp/file_len
	// <file_name> + 2 + 3 + 1 + 13
	int command_length = strlen(filename) + 20;
	char * command = (char *) calloc(command_length, sizeof(char *));
	sprintf(command, "wc %s > /tmp/file_len", filename);

	system(command);

	free(command);

	// Opening tmp file

	FILE * tmp_file = fopen("/tmp/file_len", "r");
	if (tmp_file == NULL) return;

	// Get file length
	fseek(tmp_file, 0, SEEK_END);
	long file_length = ftell(tmp_file);

	// Reset pointer on tmp_file
	fseek(tmp_file, 0, SEEK_SET);

	// Populate data
	tab->blocks[tab->cur_items] = calloc(file_length, sizeof(char *));
	fread(tab->blocks[tab->cur_items], 1, file_length, tmp_file);
	
	fclose(tmp_file);
	tab->cur_items++;

	// Remove tmp_file
	remove("/tmp/file_len");

	return;
}

char * block_content(int index, Table * tab) {
	if (index >= tab->cur_items) return NULL;

	return tab->blocks[index];
}

// TODO: Poprawić remove_block (zeby przesuwala wszystko itp.)
void remove_block(int index, Table * tab) {
	if (index >= tab->cur_items) return;

	free(tab->blocks[index]);

	return;
}

void free_table(Table * tab) {
	for (int i = 0; i < tab->cur_items; i++){
		free(tab->blocks[i]);
	}

	tab->cur_items = 0;
	return;
}