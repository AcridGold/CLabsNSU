#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze.h"

#define WALL '#'
#define PATH '.'
#define START 'A'
#define END 'B'

void maze_gen(int N, int M)
{
    srand(time(NULL));

    char **maze = malloc(N * sizeof(char*));
    if (!maze)
    {
        printf("MEMORY!\n");
        return;
    }
    for (int i = 0; i < N; i++)
    {
        maze[i] = malloc(M * sizeof(char));
        if (!maze[i])
        {
            printf("MEMORY!\n");
            for (int j = 0; j < i; j++) free(maze[j]);
            free(maze);
            return;
        }
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            maze[i][j] = WALL;

    int x = M / 2;
    int y = N / 2;
    maze[y][x] = START; // Стартовая точка

    int steps = (N * M) / 2; // Количество шагов
    int directions[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

    for (int i = 0; i < steps; i++)
    {
        int dir = rand() % 4;
        int new_x = x + directions[dir][0];
        int new_y = y + directions[dir][1];

        // Границы чек
        if (new_x > 0 && new_x < M - 1 && new_y > 0 && new_y < N - 1)
        {
            x = new_x;
            y = new_y;
            if (maze[y][x] != START){
                maze[y][x] = PATH;
            }
        }
    }

    int max_dist = 0;
    int end_x = x, end_y = y;
    for (int i = 1; i < N - 1; i++)
    {
        for (int j = 1; j < M - 1; j++)
        {
            if (maze[i][j] == PATH)
            {
                int dist = abs(i - (N / 2)) + abs(j - (M / 2));
                if (dist > max_dist)
                {
                    max_dist = dist;
                    end_x = j;
                    end_y = i;
                }
            }
        }
    }

    maze[end_y][end_x] = END; // Ставим точку B

    // ~~~ Сохранение лабиринта в файл ~~~
    FILE *file = fopen("maze.txt", "w");
    if (!file)
    {
        printf("Writing error!\n");
        for (int i = 0; i < N; i++) free(maze[i]);
        free(maze);
        return;
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            fputc(maze[i][j], file);
        }
        fputc('\n', file);
    }

    fclose(file);
    printf("OUTPUT: maze.txt\n");

    for (int i = 0; i < N; i++)
    {
        free(maze[i]);
    }
    free(maze);
}
