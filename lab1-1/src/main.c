#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_PATTERN_LENGTH 16
#define BLOCK_SIZE 1048576  // Размер блока для чтения

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
    long long int text_hash = compute_hash(text, 0, pattern_length);
    long long int power = pow(3, pattern_length - 1);

    for (size_t i = 0; i <= text_length - pattern_length; i++)
    {
        if (i > 0)
        {
            text_hash = (text_hash - (unsigned int)text[i - 1] % 3) / 3 + (unsigned int)text[i + pattern_length - 1] % 3 * power;
        }

        if (pattern_hash == text_hash)
        {
            for (size_t j = 0; j < pattern_length; j++)
            {
                printf("%llu ", global_index + i + j + 1);
                if (pattern[j] != text[i + j]) break;
            }
        }
    }
    global_index += text_length;
}

int main(int argc, char* argv[])
{
    time_t start_time;
    start_time = clock();
    FILE* file = NULL;
    if (argc == 1)
    {
        char pattern[MAX_PATTERN_LENGTH + 1] = {0};
        char buffer[BLOCK_SIZE + 1] = {0};
        // Ввод паттерна
        if (scanf("%16[^\n]%*c", pattern) == EOF) return EXIT_SUCCESS;

        // Считаем хэш паттерна
        size_t pattern_length = strlen(pattern);
        long long int pattern_hash = compute_hash(pattern, 0, pattern_length);
        if (pattern_hash == 0)
        {
            printf("%lld", pattern_hash);
            return EXIT_SUCCESS;
        }

        printf("%lld ", pattern_hash);

        while (fgets(buffer, sizeof(buffer), stdin) != 0)
        {
            size_t bytes_read = strlen(buffer);
            process_text(pattern, buffer, pattern_length, pattern_hash, bytes_read);
        }
    }
    else
    {
        char buffer[BLOCK_SIZE + 1] = {0};
        if (argc != 3)
        {
            printf("\nUsage: lab4.exe [PATTERN] [FILE]\n");
            return EXIT_SUCCESS;
        }
        file = fopen(argv[2], "r");

        if (file == NULL)
        {
            printf("Wrong file Path: '%s'\n", argv[2]);
            return 0;
        }

        // Считаем хэш паттерна
        size_t pattern_length = strlen(argv[1]);
        long long int pattern_hash = compute_hash(argv[1], 0, pattern_length);
        if (pattern_hash == 0)
        {
            printf("%lld", pattern_hash);
            goto cleanup; // Переходим к закрытию файла
        }

        printf("%lld ", pattern_hash);

        while (fread(buffer, sizeof(char), 1024, file) != 0)
        {
            size_t bytes_read = strlen(buffer);
            process_text(argv[1], buffer, pattern_length, pattern_hash, bytes_read);
        }
        if (fclose(file) == EOF)
        {
            printf("Unable to close file\n");
            return EXIT_SUCCESS;
        }
    cleanup:
        if (fclose(file) == EOF)
        {
            printf("Error closing file\n");
            return EXIT_SUCCESS;
        }
        time_t end_time;
        end_time = clock();
        printf("\n%ld ms\n", end_time - start_time);
    }
    return EXIT_SUCCESS;
}
