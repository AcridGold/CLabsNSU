#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define INF LLONG_MAX

/* Алгоритм Дейкстра прекрасно расписан в 3-ем издании CLRS со всей его подноготной (658 стр.)
   Здесь -> https://github.com/QSaman/CLRS/blob/master/3rdEdition/CLRS_3rd_Edition.pdf с 58стр. видно, как реализация влияет на время
   Была идея использовать в алгоритме фиббоначиевы кучи, описанные в статье Ясинского (https://www.elibrary.ru/item.asp?id=38537670),
   но в случае больших входных данных эффективности не добавляет.
   Еще один наглядный пример работы алгоритма Дейкстра можно найти здесь -> https://www.elibrary.ru/item.asp?id=35468863 */

typedef struct
{
    unsigned int bits[(5000 + 31) / 32];
} Bitset;

Bitset* InitBitset()
{
    Bitset* bs = (Bitset*)malloc(sizeof(Bitset));
    if (bs) memset(bs->bits, 0, sizeof(bs->bits)); // Зануляем выделенную память
    return bs;
}

/* Интерпретация Bitset из C++.
   Референс Bitset https://www.geeksforgeeks.org/cpp-bitset-and-its-application/
   Bitset использует в ~10 раз меньше памяти, чем bool-массив. + он быстрее из-за использования побитовых операций
   Это позволяет проходить тест №50. */

void FreeBitset(Bitset* bs)
{
    if (bs)
    {
        free(bs);
    }
}

void SetBitsetBit(Bitset* bs, int index)
{
    if (bs && index > 0)
    {
        index--;
        /* Преобразование в 0-based индексацию, т.к. на входе index принадлежит 1-based индексации.
         Что значит индексы идут не с 0 до N-1, а с 1 до N. */
        bs->bits[index / 32] |= (1 << (index % 32));
    }
}

int GetBitsetBit(const Bitset* bs, int index)
{
    if (bs && index > 0)
    {
        index--;
        /* Преобразование в 0-based индексацию, т.к. на входе index принадлежит 1-based индексации.
         Что значит индексы идут не с 0 до N-1, а с 1 до N. */
        return (bs->bits[index / 32] & (1 << (index % 32))) != 0;
    }
    return 0;
}

void dijkstra(int N, int S, int F, int** graph)
{
    long long* dist = (long long*)malloc((N + 1) * sizeof(long long));
    int* prev = (int*)malloc((N + 1) * sizeof(int));
    int* countPaths = (int*)malloc((N + 1) * sizeof(int));
    Bitset* visited = InitBitset();

    if (!dist || !prev || !countPaths || !visited)
    {
        free(dist);
        free(prev);
        free(countPaths);
        FreeBitset(visited);
        return; // Ошибка выделения памяти
    }

    // Подготовка
    for (int i = 1; i <= N; i++)
    {
        dist[i] = INF;
        prev[i] = -1;
        countPaths[i] = 0;
    }
    dist[S] = 0;
    countPaths[S] = 1;

    // Перебор
    for (int count = 0; count < N; count++)
    {
        int u = -1;
        long long minDist = INF;
        for (int v = 1; v <= N; v++)
        {
            if (!GetBitsetBit(visited, v) && dist[v] < minDist)
            {
                minDist = dist[v];
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
                    countPaths[v] = countPaths[u];
                }
                else if (dist[u] + weight == dist[v])
                {
                    countPaths[v]++;
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
    else if (dist[F] > INT_MAX && countPaths[F] > 1)
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
    free(countPaths);
    FreeBitset(visited);
}

int main()
{
    int N, S, F, M;
    int** graph = NULL;
    long long* dist = NULL;
    int* prev = NULL;
    int* countPaths = NULL;


    if (scanf("%d", &N) != 1 || N < 0 || N > 5000)
    {
        printf("bad number of vertices\n");
        goto cleanup;
    }
    if (scanf("%d %d", &S, &F) != 2 || S < 1 || S > N || F < 1 || F > N)
    {
        printf("bad vertex\n");
        goto cleanup;
    }
    if (scanf("%d", &M) != 1 || M < 0 || M > N * (N - 1) / 2)
    {
        printf("bad number of edges\n");
        goto cleanup;
    }

    // Создание графа
    graph = (int**)malloc((N + 1) * sizeof(int*));
    if (graph == NULL)
    {
        printf("Memory allocation failed\n");
        goto cleanup;
    }
    for (int i = 1; i <= N; i++)
    {
        graph[i] = (int*)malloc((N + 1) * sizeof(int));
        if (graph[i] == NULL)
        {
            printf("Memory allocation failed\n");
            for (int j = 1; j < i; j++)
            {
                free(graph[j]);
            }
            free(graph);
            graph = NULL;
            goto cleanup;
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
        if (scanf("%d %d %lld", &u, &v, &w) != 3)
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
        graph[u][v] = w;
        graph[v][u] = w;
    }

    dist = (long long*)malloc((N + 1) * sizeof(long long));
    prev = (int*)malloc((N + 1) * sizeof(int));
    countPaths = (int*)malloc((N + 1) * sizeof(int));
    if (dist == NULL || prev == NULL || countPaths == NULL)
    {
        printf("Memory allocation failed\n");
        goto cleanup;
    }

    for (int i = 1; i <= N; i++)
    {
        dist[i] = INF;
        prev[i] = -1;
        countPaths[i] = 0;
    }
    dist[S] = 0;
    countPaths[S] = 1;
    dijkstra(N, S, F, graph);

cleanup:
    // Освобождение всей выделенной памяти
    if (graph != NULL)
    {
        for (int i = 1; i <= N; i++)
        {
            if (graph[i] != NULL)
            {
                free(graph[i]);
            }
        }
        free(graph);
    }
    free(dist);
    free(prev);
    free(countPaths);
    return 0;
}
