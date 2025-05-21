#include <stdio.h>
#include <string.h>

#define MAX_N 2000
#define TOTAL_BYTES ((MAX_N * MAX_N + 7) / 8)

int N, M;
unsigned char matrix[TOTAL_BYTES];
int in_degree[MAX_N + 1];
int queue[MAX_N + 1];
int result[MAX_N + 1];
char visited[MAX_N + 1];

void init_graph() {
    memset(matrix, 0, TOTAL_BYTES);
    memset(in_degree, 0, (MAX_N + 1) * sizeof(int));
    memset(visited, 0, (MAX_N + 1) * sizeof(char));
}

void set_edge(int u, int v) {
    int index = (u - 1) * MAX_N + (v - 1);
    int byte = index / 8;
    int bit = index % 8;
    matrix[byte] |= (1 << bit);
}

int isEdge(int u, int v) {
    int index = (u - 1) * MAX_N + (v - 1);
    int byte = index / 8;
    int bit = index % 8;
    return (matrix[byte] >> bit) & 1;
}

int dfs(int v) {
    visited[v] = 1;
    for (int u = 1; u <= N; u++) {
        if (isEdge(v, u)) {
            if (!visited[u]) {
                if (dfs(u)) return 1;
            } else if (visited[u] == 1) {
                return 1;
            }
        }
    }
    visited[v] = 2;
    return 0;
}

int has_cycle() {
    memset(visited, 0, (MAX_N + 1) * sizeof(char));
    for (int i = 1; i <= N; i++) {
        if (!visited[i] && dfs(i)) {
            return 1;
        }
    }
    return 0;
}

int sort() {
    int front = 0, rear = 0;
    for (int i = 1; i <= N; i++) {
        if (in_degree[i] == 0) {
            queue[rear++] = i;
        }
    }
    int index = 0;
    while (front < rear) {
        int v = queue[front++];
        result[index++] = v;
        for (int u = 1; u <= N; u++) {
            if (isEdge(v, u)) {
                if (--in_degree[u] == 0) {
                    queue[rear++] = u;
                }
            }
        }
    }
    return index == N;
}

int main() {
    init_graph();

    if (scanf("%d", &N) != 1) {
        printf("bad number of lines\n");
        return 0;
    }
    if (N < 0 || N > MAX_N) {
        printf("bad number of vertices\n");
        return 0;
    }

    if (scanf("%d", &M) != 1) {
        printf("bad number of lines\n");
        return 0;
    }
    if (M < 0 || M > N * (N - 1) / 2) {
        printf("bad number of edges\n");
        return 0;
    }

    for (int i = 0; i < M; i++) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            printf("bad number of lines\n");
            return 0;
        }
        if (u < 1 || u > N || v < 1 || v > N) {
            printf("bad vertex\n");
            return 0;
        }
        set_edge(u, v);
        in_degree[v]++;
    }

    if (has_cycle()) {
        printf("impossible to sort\n");
        return 0;
    }

    if (sort()) {
        for (int i = 0; i < N; i++) {
            printf("%d", result[i]);
            if (i < N - 1) printf(" ");
        }
        printf("\n");
    } else {
        printf("impossible to sort\n");
    }

    return 0;
}
