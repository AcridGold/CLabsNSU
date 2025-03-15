#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
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
    if (BtSt) memset(BtSt->bits, 0, sizeof(BtSt->bits)); // Зануляем выделенную память
    return BtSt;
}

/* Интерпретация Bitset из C++.
   Референс Bitset https://www.geeksforgeeks.org/cpp-bitset-and-its-application/
   Bitset использует в ~10 раз меньше памяти, чем bool-массив. + он быстрее из-за использования побитовых операций
   Это позволяет проходить тест №50. */

void FreeBitset(Bitset* BtSt)
{
    if (BtSt)
    {
        free(BtSt);
    }
}

void SetBitsetBit(Bitset* BtSt, int index)
{
    if (BtSt && index > 0)
    {
        index--;
        /* Преобразование в 0-based индексацию, т.к. на входе index принадлежит 1-based индексации.
         Что значит индексы идут не с 0 до N-1, а с 1 до N. */
        BtSt->bits[index / 32] |= (1 << (index % 32));
    }
}

int GetBitsetBit(const Bitset* BtSt, int index)
{
    if (BtSt && index > 0)
    {
        index--;
        /* Преобразование в 0-based индексацию, т.к. на входе index принадлежит 1-based индексации.
         Что значит индексы идут не с 0 до N-1, а с 1 до N. */
        return (BtSt->bits[index / 32] & (1 << (index % 32))) != 0;
    }
    return 0;
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
        FreeBitset(visited);
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
            if (!GetBitsetBit(visited, v) && dist[v] < MinDist)
            {
                MinDist = dist[v];
                u = v;
            }
        }
        if (u == -1)
        {
            break;
        }
        SetBitsetBit(visited, u);
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
    FreeBitset(visited);
}

int main(int argc, char* argv[])
{
    FILE* in = NULL;
    int N, S, F, M;
    int** graph = NULL;
    long long* dist = NULL;
    int* prev = NULL;
    int* CountPaths = NULL;

    // Обработка аргументов командной строки
    if (argc == 3) // ./program <X> <Y>
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
    else // ./program или ./program <input_file>
    {
        in = fopen((argc == 2) ? argv[1] : "in.txt", "r");
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
    prev = (int*)malloc((N + 1) * sizeof(int));
    CountPaths = (int*)malloc((N + 1) * sizeof(int));
    if (!dist || !prev || !CountPaths)
    {
        printf("Memory allocation failed\n");
        goto cleanup;
    }

    for (int i = 1; i <= N; i++)
    {
        dist[i] = INF;
        prev[i] = -1;
        CountPaths[i] = 0;
    }
    dist[S] = 0;
    CountPaths[S] = 1;
    dijkstra(N, S, F, graph);

cleanup:
    if (graph)
    {
        for (int i = 1; i <= N; i++)
            if (graph[i]) free(graph[i]);
        free(graph);
    }
    free(dist);
    free(prev);
    free(CountPaths);
    if (in) fclose(in);
    return 0;
}
