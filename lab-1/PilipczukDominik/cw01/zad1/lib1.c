#include <stdlib.h>

typedef struct Tablica {
	void * t;
	int max_items;
	int cur_items;
} Tablica;

Tablica tab_init(int size){
	Tablica tab;
	tab.cur_items = 0;
	tab.max_items = size;
	tab.t = calloc()
}