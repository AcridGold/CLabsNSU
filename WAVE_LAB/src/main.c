#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

#define MAX_QUEUE 1024

// Функция sleep() на миллисекунды
#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms) // Windows использует Sleep()
#else
    #include <unistd.h>
    #define SLEEP(ms) usleep((ms) * 1000) // Linux/macOS используют usleep()
#endif

// Очистить экран
void Clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Все ОК
void Print_Table(int Y, int X, char table[Y][X])
{
    for (int i = 0; i < Y; i++)
    {
        for (int j = 0; j < X; j++)
        {
            printf("%c", table[i][j]);
        }
        printf("\n");
    }
}

// Все ОК
void SetStartPoint(int point[2], const int Y, const int X, char table[Y][X])
{
    for (int i = 0; i < Y; i++)
    {
        for (int j = 0; j < X; j++)
        {
            if (table[i][j] == 'A')
            {
                point[0] = j;
                point[1] = i;
                return;
            }
        }
    }
}

// Все ОК
void SetEndPoint(int point[2], const int Y, const int X, char table[Y][X])
{
    for (int i = 0; i < Y; i++)
    {
        for (int j = 0; j < X; j++)
        {
            if (table[i][j] == 'B')
            {
                point[0] = j;
                point[1] = i;
                return;
            }
        }
    }
}

// Пробабли воркс
int Wave(int Y, int X, char table[Y][X], int start[2], int end[2], int visited[Y][X], int distance[Y][X])
{
    int queue[MAX_QUEUE][2];
    int front = 0, rear = 0;

    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    // Добавили начало в очередь
    queue[rear][0] = start[0];
    queue[rear][1] = start[1];
    rear++;

    visited[start[1]][start[0]] = 1; // Посетили точку

    distance[start[1]][start[0]] = 0; // Расстояние до старта = 0

    while (front < rear)
    {
        // Берем точку из очереди
        int current_x = queue[front][0];
        int current_y = queue[front][1];
        front++;

        // Если конец
        if (current_x == end[0] && current_y == end[1])
        {
            return 1; // Путь найден
        }

        // Идет по направлениям
        for (int dir = 0; dir < 4; dir++)
        {
            // Новая точка
            int new_x = current_x + directions[dir][0];
            int new_y = current_y + directions[dir][1];

            if (new_x >= 0 && new_x < X && new_y >= 0 && new_y < Y) // Если точка в таблице
            {
                if (!visited[new_y][new_x] && table[new_y][new_x] != '#') // Если это новый путь
                {
                    // Точка |--> Очередь
                    queue[rear][0] = new_x;
                    queue[rear][1] = new_y;
                    rear++;

                    // Добавляем по статам
                    visited[new_y][new_x] = 1;
                    distance[new_y][new_x] = distance[current_y][current_x] + 1;

                    // Делаю красиво (～￣▽￣)～
                    table[new_y][new_x] = '0';
                    Clear_screen();
                    Print_Table(Y, X, table);
                    SLEEP(5);
                }
            }
        }
    }

    return 0; // Путь не найден
}

// Все ОК
void PaintPath(int Y, int X, char table[Y][X], int end[2], int distance[Y][X], int directions[4][2])
{
    int path_x = end[0], path_y = end[1];
    int k = 0;

    while (distance[path_y][path_x] != 0) // С конца в начало
    {
        table[path_y][path_x] = '+'; // Заполняем "+", если точка - путь

        for (int dir = 0; dir < 4; dir++)
        {
            // Прошлая точка
            int prev_x = path_x + directions[dir][0];
            int prev_y = path_y + directions[dir][1];

            if (prev_x >= 0 && prev_x < X && prev_y >= 0 && prev_y < Y)
            {
                if (distance[prev_y][prev_x] == distance[path_y][path_x] - 1) // Если дистанция на 1 меньше
                {
                    path_x = prev_x;
                    path_y = prev_y;
                    break;
                }
            }
        }
        k++; // Счет пути
    }

    // Снова делаю красиво ƪ(˘⌣˘)ʃ
    table[end[1]][end[0]] = 'B';
    Clear_screen();
    Print_Table(Y, X, table);
    SLEEP(5);

    printf("\nTotal length - %d steps\n", k);
}

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3)
    {
        printf("USAGE: WAVE_LAB [Maze_FILE]\n");
        return 42;
    }
    if (argc == 2)
    {
        printf("Maze file: %s\n", argv[1]);
        FILE* file = fopen(argv[1], "r");
        if (file == NULL)
        {
            printf("Error opening file\n");
            return 42;
        }
        int N, M;
        if (fscanf(file, "%d", &M) == EOF || fscanf(file, "%d", &N) == EOF)
        {
            printf("Error scanning file\n");
            return 42;
        }
        printf("SIZE %dx%d", N, M);
        fscanf(file, " ");

        char arr[N][M];

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                fscanf(file, "%c", &arr[i][j]);
            }
            fscanf(file, "\n");
        }
        printf("\nInput table:\n");
        Print_Table(N, M, arr);

        Sleep(1500);

        int start[2], end[2];
        SetStartPoint(start, N, M, arr); // Старт
        SetEndPoint(end, N, M, arr); // Конец

        // Наводим красоту (¬‿¬)
        Clear_screen();
        Print_Table(N, M, arr);
        SLEEP(5);
        Clear_screen();

        int visited[N][M]; // Хранилище посещенных точек
        int distance[N][M]; // Хранилище расстояний

        // Инициализируем массивы
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                visited[i][j] = 0;
                distance[i][j] = 0;
            }
        }

        int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // Шаги

        int found = Wave(N, M, arr, start, end, visited, distance);

        if (found) // Существование пути A --> B
        {
            PaintPath(N, M, arr, end, distance, directions);
        }
        else
        {
            Clear_screen();
            Print_Table(N, M, arr);
            printf("Path not found!\n");
        }
        fclose(file);
    }
    if (argc == 3)
    {
        // Usage: .exe [N] [M]

        int N = atoi(argv[1]), M = atoi(argv[2]);
        maze_gen(N, M);
        FILE* file = fopen("maze.txt", "r");
        if (file == NULL)
        {
            printf("Error opening file\n");
            return 42;
        }
        printf("SIZE %dx%d", N, M);
        fscanf(file, " ");

        char arr[N][M];

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                fscanf(file, "%c", &arr[i][j]);
            }
            fscanf(file, "\n");
        }
        printf("\nInput table:\n");
        Print_Table(N, M, arr);

        Sleep(1500);

        int start[2], end[2];
        SetStartPoint(start, N, M, arr); // Старт
        SetEndPoint(end, N, M, arr); // Конец

        // Наводим красоту (¬‿¬)
        Clear_screen();
        Print_Table(N, M, arr);
        SLEEP(5);
        Clear_screen();

        int visited[N][M]; // Хранилище посещенных точек
        int distance[N][M]; // Хранилище расстояний

        // Инициализируем массивы
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                visited[i][j] = 0;
                distance[i][j] = 0;
            }
        }

        int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // Шаги

        int found = Wave(N, M, arr, start, end, visited, distance);

        if (found) // Существование пути A --> B
        {
            PaintPath(N, M, arr, end, distance, directions);
        }
        else
        {
            Clear_screen();
            Print_Table(N, M, arr);
            printf("Path not found!\n");
        }
        fclose(file);
    }
    return 0;
}
