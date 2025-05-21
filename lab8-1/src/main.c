#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint16_t u, v;
    uint32_t weight;
} Edge;

static uint32_t* adj;
static uint32_t minWeight[5001];
static int16_t parent[5001];
static uint8_t inMST[5001];

void initPrim(int n) {
    adj = malloc(n * n * sizeof(uint32_t));
    if (!adj && n > 0) {
        printf("Memory allocation failed\n");
        exit(0);
    }
    for (int i = 0; i < n * n; i++) {
        adj[i] = UINT32_MAX;
    }
    for (int i = 1; i <= n; i++) {
        minWeight[i] = UINT32_MAX;
        parent[i] = -1;
        inMST[i] = 0;
    }
    minWeight[1] = 0;
}

int findMinVertex(int n) {
    int u = -1;
    uint32_t min_w = UINT32_MAX;
    for (int v = 1; v <= n; v++) {
        if (!inMST[v] && minWeight[v] < min_w) {
            min_w = minWeight[v];
            u = v;
        }
    }
    return u;
}

void prim(int n, Edge* mst, int* mst_size) {
    int added = 0;
    for (int i = 0; i < n; i++) {
        int u = findMinVertex(n);
        if (u == -1) break;
        inMST[u] = 1;
        if (parent[u] != -1) {
            mst[added].u = parent[u];
            mst[added].v = u;
            mst[added].weight = minWeight[u];
            added++;
        }

        for (int v = 1; v <= n; v++) {
            uint32_t weight = adj[(u - 1) * n + (v - 1)];
            if (weight != UINT32_MAX && !inMST[v] && weight < minWeight[v]) {
                minWeight[v] = weight;
                parent[v] = u;
            }
        }
    }
    *mst_size = added;
}

int main() {
    int N, M;
    if (scanf("%d", &N) != 1 || N < 0 || N > 5000) {
        printf("bad number of vertices\n");
        return 0;
    }

    if (scanf("%d", &M) != 1) {
        printf("bad number of lines\n");
        return 0;
    }

    long long max_edges = (long long)N * (N - 1) / 2;
    if (M < 0 || M > max_edges) {
        printf("bad number of edges\n");
        return 0;
    }

    if (N == 0) {
        printf(M == 0 ? "no spanning tree\n" : "bad number of edges\n");
        return 0;
    }

    initPrim(N);

    for (int i = 0; i < M; i++) {
        uint16_t u, v;
        long long w;
        if (scanf("%hu %hu %lld", &u, &v, &w) != 3) {
            printf("bad number of lines\n");
            free(adj);
            return 0;
        }
        if (u < 1 || u > N || v < 1 || v > N) {
            printf("bad vertex\n");
            free(adj);
            return 0;
        }
        if (w < 0 || w > INT32_MAX) {
            printf("bad length\n");
            free(adj);
            return 0;
        }
        if (u != v) {
            adj[(u - 1) * N + (v - 1)] = (uint32_t)w;
            adj[(v - 1) * N + (u - 1)] = (uint32_t)w;
        }
    }

    Edge* mst = (Edge*)malloc((N - 1) * sizeof(Edge));
    if (!mst && N > 1) {
        printf("Memory allocation failed\n");
        free(adj);
        return 0;
    }
    int mst_size = 0;

    prim(N, mst, &mst_size);

    if (mst_size != N - 1) {
        printf("no spanning tree\n");
    } else {
        for (int i = 0; i < mst_size; i++) {
            printf("%d %d\n", mst[i].u, mst[i].v);
        }
    }

    free(adj);
    free(mst);
    return 0;
}
