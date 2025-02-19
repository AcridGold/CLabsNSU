#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int main() {

    int N;

    if (scanf("%d", &N) == 0) {
        printf("bad input");
        return EXIT_SUCCESS;
    }

    if (N < 1 || N > 10) {
        printf("bad input");
        return EXIT_SUCCESS;
    }

    long long int arr[N];

    for (int i = 0; i < N; i++) {
        if (scanf("%lli", &arr[i]) == EOF) {
            printf("bad input");
            return EXIT_SUCCESS;
        }
    }

    for (int i = 0; i < N; i++) {
        if (arr[i] < INT_MIN || arr[i] > INT_MAX){
            printf("bad input");
            return EXIT_SUCCESS;
        }
    }

    int arr_size = (sizeof(arr) / sizeof(arr[0]));

    if (arr_size != N) {
        printf("bad input");
        return EXIT_SUCCESS;
    }

    long int sum = 0;

    for (int i = 0; i < N; i++) {
        sum += arr[i];
    }

    if (!(INT_MIN <= sum && sum <= INT_MAX)) {
        printf("overflow");
        return EXIT_SUCCESS;
    }

    printf("%li\n", sum);
    return EXIT_SUCCESS;
}
