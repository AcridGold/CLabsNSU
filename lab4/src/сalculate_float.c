#include "Calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

// Инициализация стека для чисел с плавающей точкой
void initFloatStack(FloatStack* stack) {
    stack->top = -1;
}

int isFloatStackEmpty(FloatStack* stack) {
    return stack->top == -1;
}

void pushFloat(FloatStack* stack, double value) {
    if (stack->top < MAX_INPUT - 1) {
        stack->data[++stack->top] = value;
    }
}

double popFloat(FloatStack* stack) {
    if (!isFloatStackEmpty(stack)) {
        return stack->data[stack->top--];
    }
    return -1;
}

void replace_dollar_with_memory(char* str, double memory) {
    char temp[MAX_INPUT + 1];
    char num_str[40];
    sprintf(num_str, "%.2f", memory);
    int k = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '$') {
            int len = strlen(num_str);
            if (k + len >= MAX_INPUT) break;
            strcpy(&temp[k], num_str);
            k += len;
        } else {
            if (k >= MAX_INPUT - 1) break;
            temp[k++] = str[i];
        }
    }
    temp[k] = '\0';
    strcpy(str, temp);
}



// Преобразование инфиксного выражения в постфиксное (для float)
int inf_to_post_float(const char* str, char* post) {
    Stack stack;
    initStack(&stack);
    int k = 0;
    int waitOperator = 1;

    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (isspace(c)) continue;

        // Числа с плавающей точкой
        if (isdigit(c) || c == '.') {
            int has_dot = (c == '.');
            while (isdigit(str[i]) || (str[i] == '.' && !has_dot)) {
                if (str[i] == '.') {
                    has_dot = 1;
                }
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = str[i++];
            }
            if (k >= MAX_POSTFIX) return 0;
            post[k++] = ' ';
            i--;
            waitOperator = 0;
        }
        else if (c == '(') {
            push(&stack, c);
            waitOperator = 1;
        }
        else if (c == ')') {
            if (waitOperator) return 0;
            while (!isStackEmpty(&stack) && peek(&stack) != '(') {
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = pop(&stack);
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = ' ';
            }
            if (isStackEmpty(&stack)) return 0;
            pop(&stack);
        }
        else if (isOperator(c)) {
            if (waitOperator) return 0;
            while (!isStackEmpty(&stack) &&
                   (priority(peek(&stack)) > priority(c) ||
                    (priority(peek(&stack)) == priority(c) && c != '^'))) {
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = pop(&stack);
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = ' ';
            }
            push(&stack, c);
            waitOperator = 1;
        }
        else {
            return 0;
        }
    }
    if (waitOperator) return 0;
    while (!isStackEmpty(&stack)) {
        if (peek(&stack) == '(') return 0;
        if (k >= MAX_POSTFIX) return 0;
        post[k++] = pop(&stack);
        if (k >= MAX_POSTFIX) return 0;
        post[k++] = ' ';
    }
    if (k >= MAX_POSTFIX) return 0;
    post[k] = '\0';
    return 1;
}

// Вычисление выражения в постфиксной нотации для чисел с плавающей точкой
int evaluate_post_float(const char* post, double* result) {
    FloatStack stack;
    initFloatStack(&stack);

    for (int i = 0; post[i] != '\0'; i++) {
        if (isspace(post[i])) continue;
        if (isdigit(post[i]) || post[i] == '.') {
            double num = 0;
            int decimal = 0;
            double factor = 0.1;
            while (isdigit(post[i]) || post[i] == '.') {
                if (post[i] == '.') {
                    decimal = 1;
                } else {
                    if (decimal) {
                        num += (post[i] - '0') * factor;
                        factor *= 0.1;
                    } else {
                        num = num * 10 + (post[i] - '0');
                    }
                }
                i++;
            }
            pushFloat(&stack, num);
            i--;
        } else if (isOperator(post[i])) {
            if (stack.top < 1) {
                return 0;
            }
            double b = popFloat(&stack);
            double a = popFloat(&stack);
            switch (post[i]) {
                case '+': pushFloat(&stack, a + b); break;
                case '-': pushFloat(&stack, a - b); break;
                case '*': pushFloat(&stack, a * b); break;
                case '/':
                    if (b == 0) return -1;
                    pushFloat(&stack, a / b);
                    break;
                case '^': pushFloat(&stack, pow(a, b)); break;
                default: return 0;
            }
        } else {
            return 0;
        }
    }
    if (stack.top != 0) {
        return 0;
    }
    *result = popFloat(&stack);
    return 1;
}
