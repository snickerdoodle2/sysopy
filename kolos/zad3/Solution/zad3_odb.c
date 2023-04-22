#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

struct pak{
 int i;
 char lit;
};

int main (int lpar, char *tab[]){
  int w1;
  struct pak ob1;
  int res = mkfifo("potok1", 0666);

  while (1){
  	FILE * potok = fopen("potok1", "r");
  	if (potok == NULL) return 1;
    void * buf = malloc(sizeof(struct pak));
    int d = fread(buf, (sizeof (struct pak)), 1, potok);
    ob1 = *( struct pak *) buf;
  	fclose(potok);
	if (d > 0){
    printf("otrzymano: %d %c\n",ob1.i,ob1.lit); fflush(stdout);
	}
  }


  return 0;
}
