#include <stdio.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Массив -> бинарная куча
void heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest]){
        largest = left;
    }
    if (right < n && arr[right] > arr[largest]){
        largest = right;
    }
    if (largest != i) {
        swap(&arr[i], &arr[largest]);

        // Рекурсией преобразуем в дерево:
        heapify(arr, n, largest);
    }
}

void heapSort(int arr[], int n) {
    // Array regrouping
    for (int i = n / 2 - 1; i >= 0; i--){
        heapify(arr, n, i);
    }

    // Извлечение элементов
    for (int i = n - 1; i > 0; i--) {
        // Перемещаем текущий элемент в конец
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0); // На уменьшенной куче
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; ++i){
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int n;
    if (scanf("%d\n", &n) == EOF || n==0){
        return 0;
    }
    int arr[n];
    for (int i = 0; i < n; i++) {
        if (scanf("%d\n", &arr[i]) == EOF || n==0){
            return 0;
        }
    }

    heapSort(arr, n);

    printArray(arr, n);
    return 0;
}
