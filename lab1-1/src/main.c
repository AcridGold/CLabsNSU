#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATTERN_LENGTH 100
#define BLOCK_SIZE 500  // Размер блока для чтения

// void swap(int* a, int* b)
// {
//     int t = *a;
//     *a = *b;
//     *b = t;
// }
//
// // Копирование массива
// void copy_array(char* from, char* to, size_t len)
// {
//     for (size_t i = 0; i < len; i++)
//         swap(&to[i], &from[i]);
// }

// Вычисление хэша
long long int compute_hash(const char str[], const size_t i0, const size_t i1)
{
    long long int hash = 0, power = 1;
    for (size_t i = i0; i < i1; i++)
    {
        hash += (unsigned int)str[i] % 3 * power;
        power *= 3;
    }
    return hash;
}

unsigned long long int global_index = 0;

// Ищет вхождения
void process_text(const char* pattern, const char* text, size_t pattern_length, long long int pattern_hash,
                  size_t text_length)
{
    for (size_t i = 0; i <= text_length - pattern_length; i++)
    {
        long long int text_hash = compute_hash(text, i, i + pattern_length);
        if (pattern_hash == text_hash)
        {
            for (size_t j = 0; j < pattern_length; j++)
            {
                printf("%llu ", global_index + i + j + 1);
                for (size_t k = 0; k < pattern_length; k++) printf("%c", text[i + j + k]);
                if (pattern[j] != text[i + j]) break;
            }
        }
    }
    global_index += text_length;
}

int main(int argc, char* argv[])
{
    // argc - элементов на ввод, argv - массив значений
    //char pattern[MAX_PATTERN_LENGTH + 1] = {0};
    char buffer[BLOCK_SIZE + 1] = {0};
    if (argc != 2) printf("Usage:\n[PATTERN] + [FILE]\n");
    FILE* file = fopen(argv[2], "r");

    // Перенос паттерна
    //copy_array(argv[1], pattern, strlen(argv[1]));

    // Считаем хэш паттерна
    size_t pattern_length = strlen(argv[1]);
    long long int pattern_hash = compute_hash(argv[1], 0, pattern_length);
    if (pattern_hash == 0)
    {
        printf("%lld", pattern_hash);
        return EXIT_SUCCESS;
    }

    printf("%lld ", pattern_hash);

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        size_t bytes_read = strlen(buffer);
        process_text(argv[1], buffer, pattern_length, pattern_hash, bytes_read);
    }

    return EXIT_SUCCESS;
}
