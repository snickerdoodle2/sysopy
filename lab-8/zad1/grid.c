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

const int grid_width = 30;
const int grid_height = 30;

char * local_grid;

pthread_t * grid_threads;

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
    coords my_coords = *(coords *) args;
    printf("x: %d y: %d\n", my_coords.x, my_coords.y);
    signal(SIGUSR1, signal_handler);
    while (1) {
        pause();
        printf("x: %d y: %d\n", my_coords.x, my_coords.y);
        fflush(stdout);
    }
}

void create_threads() {
    grid_threads = calloc(grid_width * grid_height, sizeof(pthread_t));
    coords * tmp = calloc(grid_width * grid_height, sizeof(coords));
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            tmp[i * grid_width + j].x = i;
            tmp[i * grid_width + j].y = j;
            pthread_create(&grid_threads[i * grid_width + j], NULL, &thread_handler, (void *) &tmp[i * grid_width + j]);
        }
    }   
    sleep(1);
    free(tmp);
}

void destroy_threads() {
    free(grid_threads);
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
            if (local_grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (local_grid[row * grid_width + col])
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
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            pthread_kill(grid_threads[i * grid_width + j], SIGUSR1);
        }
    }   
    local_grid = src;
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j);
        }
    }
}
