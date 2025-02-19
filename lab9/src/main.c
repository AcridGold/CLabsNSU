#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define INF LLONG_MAX

typedef struct {
    int to;
    long long weight;
} Edge;

typedef struct {
    int vertex;
    long long dist;
} Node;

typedef struct {
    Node* nodes;
    int size;
} PriorityQueue;

void swap(Node* a, Node* b) {
    Node temp = *a;
    *a = *b;
    *b = temp;
}

void push(PriorityQueue* pq, Node node) {
    pq->nodes[pq->size++] = node;
    int i = pq->size - 1;
    while (i > 0 && pq->nodes[i].dist < pq->nodes[(i - 1) / 2].dist) {
        swap(&pq->nodes[i], &pq->nodes[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

Node pop(PriorityQueue* pq) {
    Node root = pq->nodes[0];
    pq->nodes[0] = pq->nodes[--pq->size];
    int i = 0;
    while (2 * i + 1 < pq->size) {
        int left = 2 * i + 1, right = 2 * i + 2, minIdx = left;
        if (right < pq->size && pq->nodes[right].dist < pq->nodes[left].dist)
            minIdx = right;
        if (pq->nodes[i].dist <= pq->nodes[minIdx].dist)
            break;
        swap(&pq->nodes[i], &pq->nodes[minIdx]);
        i = minIdx;
    }
    return root;
}

void dijkstra(int N, int S, int F, Edge* graph[], int* edgesCount) {
    long long dist[N + 1];
    int prev[N + 1];
    bool visited[N + 1];
    int countPaths[N + 1];

    for (int i = 1; i <= N; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = false;
        countPaths[i] = 0;
    }

    dist[S] = 0;
    countPaths[S] = 1;
    PriorityQueue pq = { malloc(N * sizeof(Node)), 0 };
    push(&pq, (Node){S, 0});

    while (pq.size > 0) {
        Node current = pop(&pq);
        if (visited[current.vertex]) continue;
        visited[current.vertex] = true;

        for (int i = 0; i < edgesCount[current.vertex]; i++) {
            Edge edge = graph[current.vertex][i];
            if (dist[current.vertex] != INF) {
                if (dist[current.vertex] + edge.weight < dist[edge.to]) {
                    dist[edge.to] = dist[current.vertex] + edge.weight;
                    prev[edge.to] = current.vertex;
                    countPaths[edge.to] = countPaths[current.vertex];
                    push(&pq, (Node){edge.to, dist[edge.to]});
                } else if (dist[current.vertex] + edge.weight == dist[edge.to]) {
                    countPaths[edge.to]++;
                }
            }
        }
    }

    free(pq.nodes);

    for (int i = 1; i <= N; i++) {
        if (dist[i] == INF) printf("oo ");
        else if (dist[i] > INT_MAX) printf("INT_MAX+ ");
        else printf("%lld ", dist[i]);
    }
    printf("\n");

    if (dist[F] == INF) {
        printf("no path\n");
    } else if (dist[F] > INT_MAX && countPaths[F] > 1) {
        printf("overflow\n");
    } else {
        int path[N], pathSize = 0;
        for (int v = F; v != -1; v = prev[v]) {
            path[pathSize++] = v;
        }
        for (int i = pathSize - 1; i >= 0; i--) printf("%d ", path[i]);
        printf("\n");
    }
}

int main() {
    int N, S, F, M;
    if (scanf("%d", &N) != 1 || N < 0 || N > 5000) {
        printf("bad number of vertices\n");
        return 0;
    }
    if (scanf("%d %d", &S, &F) != 2 || S < 1 || S > N || F < 1 || F > N) {
        printf("bad vertex\n");
        return 0;
    }
    if (scanf("%d", &M) != 1 || M < 0 || M > N * (N - 1) / 2) {
        printf("bad number of edges\n");
        return 0;
    }

    Edge* graph[N + 1];
    int edgesCount[N + 1];

    for (int i = 1; i <= N; i++) {
        graph[i] = malloc(N * sizeof(Edge));
        edgesCount[i] = 0;
    }

    for (int i = 0; i < M; i++) {
        int u, v;
        long long w;
        if (scanf("%d %d %lld", &u, &v, &w) != 3) {
            printf("bad number of lines\n");
            return 0;
        }
        if (u < 1 || u > N || v < 1 || v > N) {
            printf("bad vertex\n");
            return 0;
        }
        if (w < 0 || w > INT_MAX) {
            printf("bad length\n");
            return 0;
        }
        graph[u][edgesCount[u]++] = (Edge){v, w};
        graph[v][edgesCount[v]++] = (Edge){u, w};
    }

    dijkstra(N, S, F, graph, edgesCount);

    for (int i = 1; i <= N; i++) free(graph[i]);
    return 0;
}
