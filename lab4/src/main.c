#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Calc.h"

int main(int argc, char* argv[]) {
    char post[MAX_POSTFIX];

    if (argc > 2) {
        printf("Too many parameters or incorrect usage.\n");
        printf("Usage: %s [expression] | %s -i\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        double result;
        if (strcmp(argv[1], "-i") == 0) {
            printf("\nSend EXIT to STOP\n");
            double memory = 0;
            while (1) {
                printf("Enter your string: ");
                char input[MAX_INPUT];
                if (!fgets(input, sizeof(input), stdin)) {
                    printf("syntax error\n");
                    break;
                }
                replace_dollar_with_memory(input, memory);
                input[strcspn(input, "\n")] = 0;
                if (strcmp(input, "EXIT") == 0) {
                    return 0;
                }
                if (!inf_to_post_float(input, post)) {
                    printf("syntax error\n");
                    continue;
                }
                int mode = evaluate_post_float(post, &result);
                if (mode == -1) {
                    printf("division by zero\n");
                } else if (mode == 0) {
                    printf("syntax error\n");
                } else {
                    printf("%.10g\n", result);
                    memory = result;
                }
            }
        }
    } else {
        char input[MAX_INPUT];
        int result;
        if (!fgets(input, sizeof(input), stdin)) {
            printf("syntax error\n");
            return EXIT_SUCCESS;
        }
        if (!inf_to_post(input, post)) {
            printf("syntax error\n");
            return EXIT_SUCCESS;
        }
        int mode = evaluate_post(post, &result);
        if (mode == -1) {
            printf("division by zero\n");
        } else if (mode == 0) {
            printf("syntax error\n");
        } else {
            printf("%d\n", result);
        }
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}
