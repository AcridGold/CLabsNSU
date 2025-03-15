#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415

int main() {
    FILE* fin = fopen("in.txt", "r");
    if (fin == NULL) {
        printf("Wrong input file.\n");
        return 1;
    }

    int N;
    if (fscanf(fin, "%d", &N) != 1) {
        printf("Reading error.\n");
        fclose(fin);
        return 1;
    }

    int* x_coords = malloc(N * sizeof(int));
    int* y_coords = malloc(N * sizeof(int));

    if (x_coords == NULL || y_coords == NULL) {
        printf("Memory allocation error.\n");
        free(x_coords);
        free(y_coords);
        fclose(fin);
        return 1;
    }

    FILE* fout = fopen("test.html", "w");
    if (fout == NULL) {
        printf("Cannot create output file.\n");
        free(x_coords);
        free(y_coords);
        return 1;
    }

    fprintf(fout, "<!DOCTYPE html>\n");
    fprintf(fout, "<html>\n");
    fprintf(fout, "<head>\n");
    fprintf(fout, "<title>Graph Visualization</title>\n");
    fprintf(fout, "</head>\n");
    fprintf(fout, "<body>\n");

    int radius = 300;
    int width = radius * 2 + 100;
    int height = radius * 2 + 100;
    int center_x = radius + 50;
    int center_y = radius + 50;

    // Начало SVG
    fprintf(fout, "    <svg width=\"%d\" height=\"%d\">\n", width, height);

    // Размещаем вершины по кругу
    for (int i = 0; i < N; i++) {
        double angle = 2 * PI * i / N;
        int x = center_x + radius * cos(angle);
        int y = center_y + radius * sin(angle);
        x_coords[i] = x;
        y_coords[i] = y;

        fprintf(fout, "        <circle cx=\"%d\" cy=\"%d\" r=\"10\" fill=\"brown\" />\n", x, y);
        fprintf(fout, "        <text x=\"%d\" y=\"%d\" fill=\"white\" text-anchor=\"middle\" dy=\".3em\">%d</text>\n",
                x, y, i + 1);
    }

    // Отрисовка рёбер
    int u, v, len;
    while (fscanf(fin, "%d %d %d", &u, &v, &len) == 3) {
        u--;
        v--;

        if (u >= 0 && u < N && v >= 0 && v < N) {
            fprintf(fout, "    <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\" stroke-width=\"1\" />\n",
                    x_coords[u], y_coords[u], x_coords[v], y_coords[v]);

            int mid_x = (x_coords[u] + x_coords[v]) / 2;
            int mid_y = (y_coords[u] + y_coords[v]) / 2;

            fprintf(fout, "    <text x=\"%d\" y=\"%d\" fill=\"blue\" text-anchor=\"middle\" dy=\".3em\">%d</text>\n",
                    mid_x, mid_y, len);
        }
    }

    fprintf(fout, "    </svg>\n");
    fprintf(fout, "</body>\n");
    fprintf(fout, "</html>\n");

    fclose(fout);
    fclose(fin);
    free(x_coords);
    free(y_coords);

    return 0;
}
