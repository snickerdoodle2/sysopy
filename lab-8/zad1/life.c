#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
//	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	//init_grid(foreground);
    create_threads();

    printf("siema\n");
    sleep(1);
	while (false)
	{
//		draw_grid(foreground);
		usleep(1000 * 1000);

		// Step simulation
		update_grid(foreground, background);
        break;
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);
    destroy_threads();

	return 0;
}
