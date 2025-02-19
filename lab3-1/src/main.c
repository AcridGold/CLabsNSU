#include <stdio.h>
#include <stdlib.h>

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Выбор медианы для опорного элемента
int median_of_three(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    if (arr[low] > arr[mid]) {swap(&arr[low], &arr[mid]);}
    if (arr[low] > arr[high]) {swap(&arr[low], &arr[high]);}
    if (arr[mid] > arr[high]) {swap(&arr[mid], &arr[high]);}
    swap(&arr[mid], &arr[high]);
    return arr[high];
}

// Разбиение массива
int Partition(int arr[], int low, int high) {
    int pivot = median_of_three(arr, low, high);
    int i = low, j = high;
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            swap(&arr[i], &arr[j]);
            i++;
            j--;
        }
    }
    return j;
}


// Без этого не работает ЛОЛ))))
void InsSort(int arr[], int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void QuickSort(int arr[], int low, int high) {
    while (low < high) {
        if (high - low < 10) {
            InsSort(arr, low, high);
            return;
        }
        int pr = Partition(arr, low, high);
        if (pr - low < high - pr) {
            QuickSort(arr, low, pr);
            low = pr + 1;
        } else {
            QuickSort(arr, pr + 1, high);
            high = pr;
        }
    }
}

int main() {
    int SIZE;
    FILE* file = fopen("in.txt", "r");
    if (!file) {
        printf("File does not exist\n");
        return EXIT_FAILURE;
    }

    if (fscanf(file, "%d", &SIZE) != 1){
        fclose(file);
        return EXIT_FAILURE;
    }

    int* arr = malloc(SIZE * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < SIZE; i++) {
        if (fscanf(file, "%d", &arr[i]) != 1) {
            printf("Error reading element %d from file\n", i);
            free(arr);
            fclose(file);
            return EXIT_FAILURE;
        }
    }
    fclose(file);

    QuickSort(arr, 0, SIZE - 1);

    for (int i = 0; i < SIZE; i++) {
        printf("%d ", arr[i]);
    }
    free(arr);
    return 0;
}
