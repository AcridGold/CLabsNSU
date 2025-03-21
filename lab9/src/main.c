#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include "maze.h"

#define INF LLONG_MAX
#define MAX_VERTICES 5000

/* Алгоритм Дейкстра прекрасно расписан в 3-ем издании CLRS со всей его подноготной (658 стр.)
   Здесь -> https://github.com/QSaman/CLRS/blob/master/3rdEdition/CLRS_3rd_Edition.pdf с 58стр. видно, как реализация влияет на время
   Была идея использовать в алгоритме фиббоначиевы кучи, описанные в статье Ясинского (https://www.elibrary.ru/item.asp?id=38537670),
   но в случае больших входных данных эффективности не добавляет.
   Еще один наглядный пример работы алгоритма Дейкстра можно найти здесь -> https://www.elibrary.ru/item.asp?id=35468863 */

typedef struct
{
    uint_fast32_t bits[(5000 + 31) / 32]; // Флаг - посещения вершины. 5000 бит на ячейки по 32 бита в uint32
} Bitset;

Bitset* InitBitset()
{
    Bitset* BtSt = (Bitset*)malloc(sizeof(Bitset));
    if (BtSt) memset(BtSt->bits, 0, sizeof(BtSt->bits));
    return BtSt;
}

/* Интерпретация Bitset из C++.
   Референс Bitset https://www.geeksforgeeks.org/cpp-bitset-and-its-application/
   Bitset использует в ~10 раз меньше памяти, чем bool-массив. + он быстрее из-за использования побитовых операций
   Это позволяет проходить тест №50. */

// Макросы для битсета
#define FREE_BITSET(BtSt) \
 do { if (BtSt) free(BtSt); } while (0)

#define SET_BITSET_BIT(BtSt, index) \
 do { if (BtSt && (index) > 0) { \
 int idx = (index) - 1; \
 (BtSt)->bits[idx / 32] |= (1 << (idx % 32)); \
 }} while (0)

#define GET_BITSET_BIT(BtSt, index) \
 ((BtSt && (index) > 0) ? \
 (((BtSt)->bits[((index) - 1) / 32] & (1 << (((index) - 1) % 32))) != 0) : 0)

// HTML с подсветкой пути
void generateHtml(const char* outputFileName, int N, int** graph, long long* dist, int* path, int pathSize)
{
    FILE* out = fopen(outputFileName, "w");
    if (!out) {
        fprintf(stderr, "Cannot create output file %s\n", outputFileName);
        return;
    }

    fprintf(out, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(out, "    <title>Adjacency Matrix and Shortest Paths</title>\n");
    fprintf(out, "    <style>\n");
    fprintf(out, "        table { border-collapse: collapse; }\n");
    fprintf(out, "        th, td { border: 1px solid black; padding: 5px; text-align: center; }\n");
    fprintf(out, "        .path { background-color: brown; }\n"); // Стиль для ребер пути
    fprintf(out, "        .vertex-path { background-color: yellow; }\n"); // Стиль для вершин пути
    fprintf(out, "    </style>\n");
    fprintf(out, "</head>\n<body>\n");

    // Матрица смежности с путем
    fprintf(out, "    <h2>Adjacency Matrix</h2>\n    <table>\n");
    fprintf(out, "        <tr><th></th>");
    // Заголовки столбцов
    for (int i = 1; i <= N; i++) {
        int isVertexInPath = 0;
        for (int k = 0; k < pathSize; k++) {
            if (path[k] == i) {
                isVertexInPath = 1;
                break;
            }
        }
        if (isVertexInPath) {
            fprintf(out, "<th class=\"vertex-path\">%d</th>", i);
        } else {
            fprintf(out, "<th>%d</th>", i);
        }
    }
    fprintf(out, "</tr>\n");

    // Строки матрицы
    for (int i = 1; i <= N; i++) {
        int isVertexInPath = 0;
        for (int k = 0; k < pathSize; k++) {
            if (path[k] == i) {
                isVertexInPath = 1;
                break;
            }
        }
        if (isVertexInPath) {
            fprintf(out, "        <tr><th class=\"vertex-path\">%d</th>", i);
        } else {
            fprintf(out, "        <tr><th>%d</th>", i);
        }

        for (int j = 1; j <= N; j++) {
            int isPath = 0;
            // Если часть пути (ребро)
            for (int k = 0; k < pathSize - 1; k++) {
                if ((path[k] == i && path[k + 1] == j) || (path[k] == j && path[k + 1] == i)) {
                    isPath = 1;
                    break;
                }
            }
            if (isPath && graph[i][j] != 0) {
                fprintf(out, "<td class=\"path\">%d</td>", graph[i][j]);
            } else {
                fprintf(out, "<td>%d</td>", graph[i][j]);
            }
        }
        fprintf(out, "</tr>\n");
    }
    fprintf(out, "    </table>\n");

    // Таблица расстояний
    fprintf(out, "    <h2>Shortest Paths from Start Vertex</h2>\n    <table>\n");
    fprintf(out, "        <tr><th>Vertex</th><th>Distance</th></tr>\n");
    for (int i = 1; i <= N; i++) {
        fprintf(out, "        <tr><td>%d</td><td>", i);
        if (dist[i] == INF) {
            fprintf(out, "∞");
        } else if (dist[i] > INT_MAX) {
            fprintf(out, "INT_MAX+");
        } else {
            fprintf(out, "%lld", dist[i]);
        }
        fprintf(out, "</td></tr>\n");
    }
    fprintf(out, "    </table>\n");

    // Картинки
    fprintf(out, "    <img src=\"../src/media/dota2.jpeg\" alt=\"dota2 meme\" width=\"300\" height=\"200\">\n");
    fprintf(out, "    <img src=\"../src/media/eminem.jpeg\" alt=\"eminem yooo\" width=\"300\" height=\"200\">\n");
    fprintf(out, "    <img src=\"../src/media/piggy.jpeg\" alt=\"nice piggy\" width=\"300\" height=\"200\">\n");

    fprintf(out, "</body>\n</html>\n");
    fclose(out);
}

void dijkstra(int N, int S, int F, int** graph)
{
    long long* dist = (long long*)malloc((N + 1) * sizeof(long long));
    int* prev = (int*)malloc((N + 1) * sizeof(int));
    int* CountPaths = (int*)malloc((N + 1) * sizeof(int));
    Bitset* visited = InitBitset();

    if (!dist || !prev || !CountPaths || !visited)
    {
        free(dist);
        free(prev);
        free(CountPaths);
        FREE_BITSET(visited);
        return; // Ошибка выделения памяти
    }

    // Подготовка
    for (int i = 1; i <= N; i++)
    {
        dist[i] = INF;
        prev[i] = -1;
        CountPaths[i] = 0;
    }
    dist[S] = 0;
    CountPaths[S] = 1;

    // Перебор
    for (int count = 0; count < N; count++)
    {
        int u = -1;
        long long MinDist = INF;
        for (int v = 1; v <= N; v++)
        {
            if (!GET_BITSET_BIT(visited, v) && dist[v] < MinDist)
            {
                MinDist = dist[v];
                u = v;
            }
        }
        if (u == -1)
        {
            break;
        }
        SET_BITSET_BIT(visited, u);
        for (int v = 1; v <= N; v++)
        {
            if (graph[u][v] != 0)
            {
                long long weight = graph[u][v];
                if (dist[u] + weight < dist[v])
                {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                    CountPaths[v] = CountPaths[u];
                }
                else if (dist[u] + weight == dist[v])
                {
                    CountPaths[v]++;
                }
            }
        }
    }

    // Вывод расстояний
    for (int i = 1; i <= N; i++)
    {
        if (dist[i] == INF)
        {
            printf("oo ");
        }
        else if (dist[i] > INT_MAX)
        {
            printf("INT_MAX+ ");
        }
        else
        {
            printf("%lld ", dist[i]);
        }
    }
    printf("\n");

    // Вывод пути
    if (dist[F] == INF)
    {
        printf("no path\n");
    }
    else if (dist[F] > INT_MAX && CountPaths[F] > 1)
    {
        printf("overflow\n");
    }
    else
    {
        int* path = (int*)malloc(N * sizeof(int));
        if (path)
        {
            int pathSize = 0;
            for (int v = F; v != -1; v = prev[v])
            {
                path[pathSize++] = v;
            }
            for (int i = 0; i < pathSize; i++)
            {
                printf("%d ", path[i]);
            }
            printf("\n");
            free(path);
        }
        else
        {
            printf("no path\n");
        }
    }

    // Освобождение памяти
    free(dist);
    free(prev);
    free(CountPaths);
    FREE_BITSET(visited);
}

int main(int argc, char* argv[])
{
    FILE* in = NULL;
    int N, S, F, M;
    int** graph = NULL;
    long long* dist = NULL;
    int* path = NULL;
    int pathSize = 0;
    const char* inputFile = "in.txt";
    const char* outputFileName = NULL;

    if (argc == 1) // ./program без аргументов
    {
        in = fopen(inputFile, "r");
    }
    else if (argc == 2 && strcmp(argv[1], "-p") == 0) // ./program -p
    {
        in = fopen(inputFile, "r");
        outputFileName = "out.html";
    }
    else if (argc == 3 && strcmp(argv[1], "-p") == 0) // ./program -p <input_file>
    {
        inputFile = argv[2];
        in = fopen(inputFile, "r");
        outputFileName = "out.html";
    }
    else if (argc == 3 && isdigit(argv[1][0]) && isdigit(argv[2][0])) // ./program <X> <Y>
    {
        int X = atoi(argv[1]);
        int Y = atoi(argv[2]);
        if (X < 3 || Y < 3 || X * Y > MAX_VERTICES)
        {
            printf("Invalid maze dimensions: X=%d, Y=%d (must be >= 3 and X*Y <= %d)\n",
                   X, Y, MAX_VERTICES);
            return 0;
        }
        maze_gen(X, Y);
        in = fopen("maze.txt", "r");
    }

    if (!in)
    {
        printf("Cannot open input file\n");
        return 0;
    }

    if (fscanf(in, "%d", &N) != 1 || N < 0 || N > 5000)
    {
        printf("bad number of vertices\n");
        fclose(in);
        return 0;
    }
    if (fscanf(in, "%d %d", &S, &F) != 2 || S < 1 || S > N || F < 1 || F > N)
    {
        printf("bad vertex\n");
        fclose(in);
        return 0;
    }
    if (fscanf(in, "%d", &M) != 1 || M < 0 || M > N * (N - 1) / 2)
    {
        printf("bad number of edges\n");
        fclose(in);
        return 0;
    }

    graph = (int**)malloc((N + 1) * sizeof(int*));
    if (!graph)
    {
        printf("Memory allocation failed\n");
        fclose(in);
        return 0;
    }
    for (int i = 1; i <= N; i++)
    {
        graph[i] = (int*)malloc((N + 1) * sizeof(int));
        if (!graph[i])
        {
            printf("Memory allocation failed\n");
            for (int j = 1; j < i; j++) free(graph[j]);
            free(graph);
            fclose(in);
            return 0;
        }
        for (int j = 1; j <= N; j++)
        {
            graph[i][j] = 0;
        }
    }

    for (int i = 0; i < M; i++)
    {
        int u, v;
        long long w;
        if (fscanf(in, "%d %d %lld", &u, &v, &w) != 3)
        {
            printf("bad number of lines\n");
            goto cleanup;
        }
        if (u < 1 || u > N || v < 1 || v > N)
        {
            printf("bad vertex\n");
            goto cleanup;
        }
        if (w < 0 || w > INT_MAX)
        {
            printf("bad length\n");
            goto cleanup;
        }

        // Матрица смежности:
        graph[u][v] = w;
        graph[v][u] = w;
    }

    dist = (long long*)malloc((N + 1) * sizeof(long long));
    if (!dist)
    {
        printf("Memory allocation failed\n");
        goto cleanup;
    }

    dijkstra(N, S, F, graph);

    if (outputFileName) {
        for (int i = 1; i <= N; i++) {
            dist[i] = INF;
        }
        dist[S] = 0;

        int* prev = (int*)malloc((N + 1) * sizeof(int));
        int* CountPaths = (int*)malloc((N + 1) * sizeof(int));
        Bitset* visited = InitBitset();
        path = (int*)malloc(N * sizeof(int));

        if (!prev || !CountPaths || !visited || !path) {
            free(prev);
            free(CountPaths);
            FREE_BITSET(visited);
            free(path);
            goto cleanup;
        }

        for (int i = 1; i <= N; i++) {
            prev[i] = -1;
            CountPaths[i] = 0;
        }
        CountPaths[S] = 1;

        for (int count = 0; count < N; count++) {
            int u = -1;
            long long MinDist = INF;
            for (int v = 1; v <= N; v++) {
                if (!GET_BITSET_BIT(visited, v) && dist[v] < MinDist) {
                    MinDist = dist[v];
                    u = v;
                }
            }
            if (u == -1) break;
            SET_BITSET_BIT(visited, u);
            for (int v = 1; v <= N; v++) {
                if (graph[u][v] != 0) {
                    long long weight = graph[u][v];
                    if (dist[u] + weight < dist[v]) {
                        dist[v] = dist[u] + weight;
                        prev[v] = u;
                        CountPaths[v] = CountPaths[u];
                    }
                    else if (dist[u] + weight == dist[v]) {
                        CountPaths[v]++;
                    }
                }
            }
        }

        if (dist[F] != INF && dist[F] <= INT_MAX) {
            pathSize = 0;
            for (int v = F; v != -1; v = prev[v]) {
                path[pathSize++] = v;
            }
        }

        generateHtml(outputFileName, N, graph, dist, path, pathSize);

        free(prev);
        free(CountPaths);
        FREE_BITSET(visited);
        free(path);
    }

cleanup:
    if (graph) {
        for (int i = 1; i <= N; i++)
            if (graph[i]) free(graph[i]);
        free(graph);
    }
    free(dist);
    if (in) fclose(in);
    return 0;
}
