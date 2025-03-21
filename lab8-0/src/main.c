#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "svg.h"

#define WIDTH 1000
#define HEIGHT 1000
#define PI 3.1416
#define RADIUS WIDTH / 3
#define CENTER_X WIDTH / 2
#define CENTER_Y HEIGHT / 2

/* Касательно алгоритма Крускала на elibrary можно найти много научных работ, например:
 * Попов А.Ю.
 * "ИССЛЕДОВАНИЕ ВАРИАНТОВ РЕАЛИЗАЦИИ АЛГОРИТМОВ КРУСКАЛА И ПРИМА В
 * ВЫЧИСЛИТЕЛЬНОЙ СИСТЕМЕ С МНОГИМИ ПОТОКАМИ КОМАНД И ОДНИМ ПОТОКОМ ДАННЫХ"
 * (https://elibrary.ru/item.asp?id=25118371)
 * Здесь приведена информация по системному взаимодействию, алгоритмам Крускала и Прима, времени работы, оптимизации и тд.
 * Присутствует визуализация вычислений
 */

void distribute(int N, double positions[][2]) {
    for (int i = 0; i < N; i++) {
        double angle = 2.0 * PI * i / N;
        positions[i][0] = CENTER_X + RADIUS * cos(angle);
        positions[i][1] = CENTER_Y + RADIUS * sin(angle);
    }
}

typedef struct {
    uint16_t u, v; // Вершин до 5000 => помещаются в 16 бит
    uint32_t weight;
} Edge;

static uint16_t parent[5001];
static uint8_t rank[5001];

int compare(const void* a, const void* b) {
    return ((Edge*)a)->weight - ((Edge*)b)->weight;
}

void initDSU(int n) {
    for (int i = 1; i <= n; i++) {
        parent[i] = i;
        rank[i] = 0;
    }
}

uint16_t find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]); // Сжатие пути
    }
    return parent[x];
}

void unionSet(int x, int y) {
    int px = find(x);
    int py = find(y);
    if (px == py) return;
    if (rank[px] < rank[py]) {
        parent[px] = py;
    } else {
        parent[py] = px;
        if (rank[px] == rank[py]) {
            rank[px]++;
        }
    }
}

int main(int argc, char* argv[]) {
    FILE* input = stdin;
    int generate_svg = 0;

    if (argc == 3 && strcmp(argv[1], "-g") == 0) {
        input = fopen(argv[2], "r");
        if (!input) {
            printf("Error opening file: %s\n", argv[2]);
            return 0;
        }
        generate_svg = 1;
    }

    int N, M;
    if (fscanf(input, "%d", &N) != 1 || N < 0 || N > 5000) {
        printf("bad number of vertices\n");
        fclose(input);
        return 0;
    }

    if (fscanf(input, "%d", &M) != 1) {
        printf("bad number of lines\n");
        fclose(input);
        return 0;
    }

    long long max_edges = (long long)N * (N - 1) / 2;
    if (M < 0 || M > max_edges) {
        printf("bad number of edges\n");
        fclose(input);
        return 0;
    }

    if (N == 0) {
        printf(M == 0 ? "no spanning tree\n" : "bad number of edges\n");
        fclose(input);
        return 0;
    }

    Edge* edges = (Edge*)malloc(M * sizeof(Edge));
    if (!edges) {
        printf("Memory allocation failed\n");
        fclose(input);
        return 0;
    }

    for (int i = 0; i < M; i++) {
        uint16_t u, v;
        long long w;
        if (fscanf(input, "%hu %hu %lld", &u, &v, &w) != 3) {
            printf("bad number of lines\n");
            free(edges);
            fclose(input);
            return 0;
        }
        if (u < 1 || u > N || v < 1 || v > N) {
            printf("bad vertex\n");
            free(edges);
            fclose(input);
            return 0;
        }
        if (w < 0 || w > INT32_MAX) {
            printf("bad length\n");
            free(edges);
            fclose(input);
            return 0;
        }
        edges[i].u = u;
        edges[i].v = v;
        edges[i].weight = w;
    }
    // Сортировка по весу
    qsort(edges, M, sizeof(Edge), compare);

    // Инициализация "Disjoint Set Union" (DSU)
    initDSU(N);

    // Массив "Minimum Spanning Tree edges" (MST) рёбер
    Edge* mst = (Edge*)malloc((N - 1) * sizeof(Edge));
    if (!mst && N > 1) {
        printf("Memory allocation failed\n");
        free(edges);
        fclose(input);
        return 0;
    }
    int mst_size = 0;

    // Крускал
    for (int i = 0; i < M && mst_size < N - 1; i++) {
        int pr_u = find(edges[i].u);
        int pr_v = find(edges[i].v);
        if (pr_u != pr_v) {
            mst[mst_size++] = edges[i];
            unionSet(pr_u, pr_v);
        }
    }

    // Проверка на связность
    if (mst_size != N - 1) {
        printf("no spanning tree\n");
    } else {
        // Вывод
        for (int i = 0; i < mst_size; i++) {
            printf("%d %d\n", mst[i].u, mst[i].v);
        }
    }
    // Если /.exe -g [input.txt]
    if (generate_svg) {
        svg* graph_svg = svg_create(WIDTH, HEIGHT);
        if (!graph_svg) {
            printf("Error creating SVG\n");
            free(edges);
            free(mst);
            fclose(input);
            return 0;
        }

        svg_fill(graph_svg, "white");
        double positions[N][2];
        distribute(N, positions); // Координаты

        // Все рёбра с весами
        for (int i = 0; i < M; i++) {
            double x0 = positions[edges[i].u - 1][0];
            double y0 = positions[edges[i].u - 1][1];
            double x1 = positions[edges[i].v - 1][0];
            double y1 = positions[edges[i].v - 1][1];

            svg_line(graph_svg, "gray", 2, x0, y0, x1, y1);

            double text_x = (x0 + x1) / 2;
            double text_y = (y0 + y1) / 2;
            char weight_label[16];
            sprintf(weight_label, "%u", edges[i].weight);
            svg_text(graph_svg, text_x, text_y, "Arial", 20, "black", "none", weight_label);
        }

        // Выделяем каркас с весами
        for (int i = 0; i < mst_size; i++) {
            double x0 = positions[mst[i].u - 1][0];
            double y0 = positions[mst[i].u - 1][1];
            double x1 = positions[mst[i].v - 1][0];
            double y1 = positions[mst[i].v - 1][1];

            svg_line(graph_svg, "black", 5, x0, y0, x1, y1);

            double text_x = (x0 + x1) / 2;
            double text_y = (y0 + y1) / 2;
            char weight_label[16];
            sprintf(weight_label, "%u", mst[i].weight);
            svg_text(graph_svg, text_x, text_y, "Arial", 20, "blue", "none", weight_label);
        }

        for (int i = 0; i < N; i++) {
            svg_circle(graph_svg, "brown", 2, "brown", 20, positions[i][0], positions[i][1]);

            char vertex_label[8];
            sprintf(vertex_label, "%d", i + 1);
            svg_text(graph_svg, positions[i][0]-6, positions[i][1]+9, "Arial", 24, "white", "none", vertex_label);
        }

        svg_save(graph_svg, "result.svg");
        svg_free(graph_svg);
    }

    free(edges);
    free(mst);

    if (input != stdin) {
        fclose(input);
    }

    return 0;
}
