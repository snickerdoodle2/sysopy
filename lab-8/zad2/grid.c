#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
    int x;
    int y;
} coords;

typedef struct {
    int n;
    coords * data;
} coords_list;

const int grid_width = 30;
const int grid_height = 30;

char * local_src_grid;
char * local_dst_grid;

pthread_t * grid_threads;
int grid_amount;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void signal_handler(int signum) {}

void * thread_handler(void * args) {
    coords_list my_coords = *(coords_list *) args;
    signal(SIGUSR1, signal_handler);
    while (1) {
        pause();
        for (int i = 0; i < my_coords.n; i++) {
            int x = my_coords.data[i].x;
            int y = my_coords.data[i].y;
            local_dst_grid[x * grid_width + y] = is_alive(x, y);
        }
    }
}

coords_list * thread_data;

int ready_threads = 0;

void create_threads(int amount) {
    grid_amount = amount;
    grid_threads = calloc(amount, sizeof(pthread_t));
    thread_data = calloc(amount, sizeof(coords_list));
    int * amount_per_thread = calloc(amount, sizeof(int));

    int pixels = grid_width * grid_height;
    ready_threads++;

    while(pixels) {
        for (int i = 0; i < amount; i++) {
            amount_per_thread[i]++;
            pixels--;
            if (!pixels) break;
        }
    }

    int cur_x = 0;
    int cur_y = 0;

    for (int i =0; i < amount; i++) {
        thread_data[i].n = amount_per_thread[i];
        thread_data[i].data = calloc(thread_data[i].n, sizeof(coords));
        for (int j = 0; j < thread_data[i].n; j++) {
            coords tmp_coords;
            tmp_coords.x = cur_x;
            tmp_coords.y = cur_y;
            thread_data[i].data[j] = tmp_coords;
            cur_x++;
            if (cur_x == grid_width) {
                cur_x = 0;
                cur_y++;
            }
        }
    }

    for (int i = 0; i < amount; ++i)
    {
        pthread_create(&grid_threads[i], NULL, &thread_handler, (void *) &thread_data[i]);
    }

    free(amount_per_thread);
}

void destroy_threads() {
    free(grid_threads);
    for (int i = 0; i < grid_amount; i++) free(thread_data[i].data);
    free(thread_data);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col)
{
    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (local_src_grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (local_src_grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    local_src_grid = src;
    local_dst_grid = dst;

    for (int i = 0; i < grid_amount; ++i)
    {
        pthread_kill(grid_threads[i], SIGUSR1);
    }   
}
