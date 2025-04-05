#include <stdio.h>
#include <string.h>
#include <ctype.h>

int isValid(const char* str)
{
    int len = strlen(str);
    if (len > 10 || len == 0) return 0;

    int used[10] = {0};
    for (int i = 0; i < len; i++)
    {
        if (!isdigit(str[i])) return 0;
        int digit = str[i] - '0';
        if (used[digit]) return 0;
        used[digit] = 1;
    }
    return 1;
}

int permutation(char* str)
{
    int len = strlen(str);
    int i = len - 2;

    while (i >= 0 && str[i] >= str[i + 1])
    {
        i--;
    }

    if (i < 0) return 0;

    int j = len - 1;
    while (j >= 0 && str[j] <= str[i])
    {
        j--;
    }

    char temp = str[i];
    str[i] = str[j];
    str[j] = temp;

    int left = i + 1;
    int right = len - 1;
    while (left < right)
    {
        temp = str[left];
        str[left] = str[right];
        str[right] = temp;
        left++;
        right--;
    }

    return 1;
}

int main()
{
    char str[32];
    int n = 0;

    if (!fgets(str, sizeof(str), stdin))
    {
        printf("bad input\n");
        return 0;
    }

    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }

    if (scanf("%d", &n) != 1)
    {
        printf("bad input\n");
        return 0;
    }

    char tmp[32];
    if (fgets(tmp, sizeof(tmp), stdin) && !feof(stdin))
    {
        for (int i = 0; tmp[i]; i++)
        {
            if (!isspace(tmp[i]))
            {
                printf("bad input\n");
                return 0;
            }
        }
    }

    if (!isValid(str))
    {
        printf("bad input\n");
        return 0;
    }

    char current[32];
    strcpy(current, str);
    int i = 0;

    while (i < n && permutation(current))
    {
        printf("%s\n", current);
        i++;
    }

    return 0;
}
