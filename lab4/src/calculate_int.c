#include "Calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

void initStack(Stack* stack) {
    stack->top = -1;
}

int isStackEmpty(Stack* stack) {
    return stack->top == -1;
}

void push(Stack* stack, int value) {
    if (stack->top < MAX_INPUT - 1) {
        stack->data[++stack->top] = value;
    }
}

int pop(Stack* stack) {
    if (!isStackEmpty(stack)) {
        return stack->data[stack->top--];
    }
    return -1;
}

int peek(Stack* stack) {
    if (!isStackEmpty(stack)) {
        return stack->data[stack->top];
    }
    return -1;
}

int priority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

int inf_to_post(const char* str, char* post) {
    Stack stack;
    initStack(&stack);
    int k = 0;
    int waitOperator = 1;
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (isspace(c)) continue;
        if (isdigit(c)) {
            while (isdigit(str[i])) {
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = str[i++];
            }
            post[k++] = ' ';
            i--;
            waitOperator = 0;
        } else if (c == '(') {
            push(&stack, c);
            waitOperator = 1;
        } else if (c == ')') {
            if (waitOperator) return 0;
            while (!isStackEmpty(&stack) && peek(&stack) != '(') {
                post[k++] = pop(&stack);
                post[k++] = ' ';
            }
            if (isStackEmpty(&stack)) return 0;
            pop(&stack);
        } else if (isOperator(c)) {
            if (waitOperator) return 0;
            while (!isStackEmpty(&stack) &&
                   (priority(peek(&stack)) > priority(c) ||
                    (priority(peek(&stack)) == priority(c) && c != '^'))) {
                post[k++] = pop(&stack);
                post[k++] = ' ';
            }
            push(&stack, c);
            waitOperator = 1;
        } else {
            return 0;
        }
    }
    while (!isStackEmpty(&stack)) {
        if (peek(&stack) == '(') return 0;
        post[k++] = pop(&stack);
        post[k++] = ' ';
    }
    post[k] = '\0';
    return 1;
}

int evaluate_post(const char* post, int* result) {
    Stack stack;
    initStack(&stack);
    for (int i = 0; post[i] != '\0'; i++) {
        if (isspace(post[i])) continue;
        if (isdigit(post[i])) {
            int num = 0;
            while (isdigit(post[i])) {
                num = num * 10 + (post[i++] - '0');
            }
            push(&stack, num);
            i--;
        } else if (isOperator(post[i])) {
            if (stack.top < 1) return 0;
            int b = pop(&stack);
            int a = pop(&stack);
            switch (post[i]) {
                case '+': push(&stack, a + b); break;
                case '-': push(&stack, a - b); break;
                case '*': push(&stack, a * b); break;
                case '/':
                    if (b == 0) return -1;
                    push(&stack, a / b);
                    break;
                case '^':
                    push(&stack, (int)pow(a, b));
                    break;
                default: return 0;
            }
        } else {
            return 0;
        }
    }
    if (stack.top != 0) return 0;
    *result = pop(&stack);
    return 1;
}
