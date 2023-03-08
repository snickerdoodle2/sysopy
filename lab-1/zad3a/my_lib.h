typedef struct Table {
	char ** blocks;
	int max_items;
	int cur_items;
} Table;

Table * tab_init(int size);

void count_file(const char * filename, Table * tab);

char * block_content(int index, Table * tab);

void remove_block(int index, Table * tab);

void free_table(Table * tab);