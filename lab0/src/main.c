#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

char int_to_charA(const int num) {
    return num - 'A' + 10;
}

char int_to_charN(const int num) {
    return num - '0';
}

int char_to_int(const char c) {
    return c - '0';
}

int is_correct_b1_notation(const char *massive, const int size_of_massive, const int size_bef_dot, const int base) {
    int count_of_dots = 0;
    if (size_of_massive > 13 || size_of_massive == 0) {
        printf("bad input\n");
        return 0;
    }
    if (size_bef_dot == 0) {
        printf("bad input\n");
        return 0;
    }
    for (int i = 0; i < size_of_massive; i++) {
        if (massive[i] == '.') {
            count_of_dots += 1;
        }
        if (massive[i] >= '0' && massive[i] <= '9') {
            if (massive[i] - '0' >= base) {
                printf("bad input\n");
                return 0;
            }
        } else if (massive[i] >= 'A' && massive[i] <= 'F') {
            if (massive[i] - 'A' + 10 >= base) {
                printf("bad input\n");
                return 0;
            }
        } else if (massive[i] >= 'a' && massive[i] <= 'f') {
            if (massive[i] - 'a' + 10 >= base) {
                printf("bad input\n");
                return 0;
            }
        } else if (massive[i] == '.') {
            continue;
        } else {
            printf("bad input\n");
            return 0;
        }
    }
    if (count_of_dots > 1) {
        printf("bad input\n");
        return 0;
    }
    if (count_of_dots == 1 && size_of_massive == size_bef_dot + 1) {
        printf("bad input\n");
        return 0;
    }
    return 1;
}

double from_b1_to_ten(const char *massive, const int size, const int size_bef_dot, const int base) {
    double num_in_ten = 0;
    for (int i = 0; i < size_bef_dot; i++) {
        if (massive[i] >= '0' && massive[i] <= '9') {
            num_in_ten += char_to_int(massive[i]) * pow(base, size_bef_dot - i - 1);
        } else if (massive[i] >= 'A' && massive[i] <= 'F') {
            num_in_ten += (massive[i] - 'A' + 10) * pow(base, size_bef_dot - i - 1);
        } else if (massive[i] >= 'a' && massive[i] <= 'f') {
            num_in_ten += (massive[i] - 'a' + 10) * pow(base, size_bef_dot - i - 1);
        }
    }
    for (int i = size_bef_dot + 1; i < size; i++) {
        if (massive[i] >= '0' && massive[i] <= '9') {
            num_in_ten += char_to_int(massive[i]) / pow(base, i - size_bef_dot);
        } else if (massive[i] >= 'A' && massive[i] <= 'F') {
            num_in_ten += (massive[i] - 'A' + 10) / pow(base, i - size_bef_dot);
        } else if (massive[i] >= 'a' && massive[i] <= 'f') {
            num_in_ten += (massive[i] - 'a' + 10) / pow(base, i - size_bef_dot);
        }
    }
    return num_in_ten;
}

int from_ten_to_b2(const double n, const int base) {
    const long long int integer_of_n = (long long int) n;
    double aft_dot = (n - integer_of_n);
    int massive[128] = {'0'};
    char nums[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    char alph[] = {'A', 'B', 'C', 'D', 'E', 'F'};
    char answ[128] = {'0'};
    long long int quotient = integer_of_n / base;
    massive[0] = integer_of_n % base;
    int count = 1;
    for (int i = 1; quotient != 0; i++) {
        int ost = quotient % base;
        quotient /= base;
        massive[i] = ost;
        count += 1;
    }
    for (int i = count - 1; i >= 0; i--) {
        if (massive[i] >= 0 && massive[i] < 10) {
            for (unsigned int j = 0; j < sizeof(nums); j++) {
                if (int_to_charN(nums[j]) == massive[i]) {
                    answ[count - i - 1] = nums[j];
                }
            }
        } else if (massive[i] > 9 && massive[i] < 17) {
            for (unsigned int j = 0; j < sizeof(alph); j++) {
                if (int_to_charA(alph[j]) == massive[i]) {
                    answ[count - i - 1] = alph[j];
                }
            }
        }
    }

    if (aft_dot == 0) {
        printf("%s", answ);

    } else {
        int new_count = 1;
        int massive2[128] = {'0'};
        int count_of_0 = 0;
        answ[count] = '.';
        for (int i = 0; (int) aft_dot != aft_dot || count_of_0 == 2; i++) {
            aft_dot *= base;
            massive2[i] = (int) aft_dot;
            aft_dot -= (int) aft_dot;
            new_count += 1;
            if (massive2[i] == 0) {
                count_of_0 += 1;
            }
            if (new_count > 12) {
                break;
            }
        }
        for (int i = 0; i < new_count + 1; i++) {
            if (massive2[i] >= 0 && massive2[i] < 10) {
                for (unsigned int j = 0; j < sizeof(nums); j++) {
                    if (int_to_charN(nums[j]) == massive2[i]) {
                        answ[count + i + 1] = nums[j];
                    }
                }
            } else if (massive2[i] > 9 && massive2[i] < 17) {
                for (unsigned int j = 0; j < sizeof(alph); j++) {
                    if (int_to_charA(alph[j]) == massive2[i]) {
                        answ[count + i + 1] = alph[j];
                    }
                }
            }
        }
        if (new_count > 12) {
            for (int i = 0; i < count + 12 + 1; i++) {
                printf("%c", answ[i]);
            }
        } else {
            for (int i = 0; i < count + new_count; i++) {
                printf("%c", answ[i]);
            }
        }
    }
    return 1;
}

int main() {
    int b1, b2;
    char b1x[256];
    if (scanf("%d%d\n", &b1, &b2) != 2 || b1 < 2 || b1 > 16 || b2 < 2 || b2 > 16) {
        printf("bad input\n");
        return 0;
    }

    if (fgets(b1x, 256, stdin) == NULL) {
        printf("bad input\n");
        return 0;
    }

    size_t size_t = strlen(b1x);
    int size = (int) size_t - 1;
    int count_bef_dot = 0;
    for (int i = 0; i < size; i++) {
        if (b1x[i] == '.') {
            break;
        }
        count_bef_dot++;
    }
    int size_bef_dot = count_bef_dot;
    if (is_correct_b1_notation(b1x, size, size_bef_dot, b1)) {
        double b1_to_ten = from_b1_to_ten(b1x, size, size_bef_dot, b1);
        from_ten_to_b2(b1_to_ten, b2);
    }

    return 0;
}
