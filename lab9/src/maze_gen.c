// Функция доработана для сохранения путей в файл как в тестировщике
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze.h"

#define WALL '#'
#define PATH '.'
#define START 'A'
#define END 'B'
#define MAX_VERTICES 5000

// Структура для хранения ребра графа
typedef struct {
    int from;
    int to;
    int weight;
} Edge;

void maze_gen(int N, int M)
{
    // Проверка на максимальное количество вершин
    if (N * M > MAX_VERTICES) {
        printf("Error: Maze size (%d * %d = %d) exceeds maximum allowed vertices (%d)\n",
               N, M, N * M, MAX_VERTICES);
        return;
    }

    srand(time(NULL));

    // Создаем двумерный массив для отслеживания лабиринта
    char **maze = malloc(N * sizeof(char*));
    if (!maze) {
        printf("MEMORY!\n");
        return;
    }
    for (int i = 0; i < N; i++) {
        maze[i] = malloc(M * sizeof(char));
        if (!maze[i]) {
            printf("MEMORY!\n");
            for (int j = 0; j < i; j++) free(maze[j]);
            free(maze);
            return;
        }
    }

    // Инициализация стенами
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            maze[i][j] = WALL;

    // Массив для хранения ребер
    Edge *edges = malloc(N * M * 4 * sizeof(Edge));
    if (!edges) {
        printf("MEMORY!\n");
        for (int i = 0; i < N; i++) free(maze[i]);
        free(maze);
        return;
    }
    int edge_count = 0;

    int x = M / 2;
    int y = N / 2;
    maze[y][x] = START;
    int start_vertex = y * M + x + 1;

    int steps = (N * M) / 2;
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    for (int i = 0; i < steps; i++) {
        int dir = rand() % 4;
        int new_x = x + directions[dir][0];
        int new_y = y + directions[dir][1];

        if (new_x > 0 && new_x < M - 1 && new_y > 0 && new_y < N - 1) {
            int current_vertex = y * M + x + 1;
            int new_vertex = new_y * M + new_x + 1;

            if (maze[new_y][new_x] == WALL) {
                maze[new_y][new_x] = PATH;
                edges[edge_count].from = current_vertex;
                edges[edge_count].to = new_vertex;
                edges[edge_count].weight = 1;
                edge_count++;
            }
            x = new_x;
            y = new_y;
        }
    }

    // Находим конечную точку
    int max_dist = 0;
    int end_x = x, end_y = y;
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < M - 1; j++) {
            if (maze[i][j] == PATH || maze[i][j] == START) {
                int dist = abs(i - (N / 2)) + abs(j - (M / 2));
                if (dist > max_dist) {
                    max_dist = dist;
                    end_x = j;
                    end_y = i;
                }
            }
        }
    }
    int end_vertex = end_y * M + end_x + 1;
    maze[end_y][end_x] = END;

    // Запись графа в maze.txt
    FILE *graph_file = fopen("maze.txt", "w");
    if (!graph_file) {
        printf("Writing error for maze.txt!\n");
        for (int i = 0; i < N; i++) free(maze[i]);
        free(maze);
        free(edges);
        return;
    }

    fprintf(graph_file, "%d\n", N * M);
    fprintf(graph_file, "%d %d\n", start_vertex, end_vertex);
    fprintf(graph_file, "%d\n", edge_count);

    for (int i = 0; i < edge_count; i++) {
        fprintf(graph_file, "%d %d %d\n", edges[i].from, edges[i].to, edges[i].weight);
    }
    fclose(graph_file);

    // Запись рисунка лабиринта в maze_ill.txt
    FILE *ill_file = fopen("maze_ill.txt", "w");
    if (!ill_file) {
        printf("Writing error for maze_ill.txt!\n");
        for (int i = 0; i < N; i++) free(maze[i]);
        free(maze);
        free(edges);
        return;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            fputc(maze[i][j], ill_file);
        }
        fputc('\n', ill_file);
    }
    fclose(ill_file);

    printf("OUTPUT: maze.txt (graph) and maze_ill.txt (illustration)\n");

    // Очистка памяти
    for (int i = 0; i < N; i++) free(maze[i]);
    free(maze);
    free(edges);
}
