#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#define MAX_INPUT 1024
#define MAX_POSTFIX (2 * MAX_INPUT)

// Стек
typedef struct Stack
{
    int data[MAX_INPUT];
    int top;
} Stack;

// Создание стека
void initStack(Stack* stack)
{
    stack->top = -1;
}

// Пустой стек
int isStackEmpty(Stack* stack)
{
    return stack->top == -1;
}

// Добавить в стек
void push(Stack* stack, int value)
{
    if (stack->top < MAX_INPUT - 1)
    {
        stack->data[++stack->top] = value;
    }
}

// Берем из стека
int pop(Stack* stack)
{
    if (!isStackEmpty(stack))
    {
        return stack->data[stack->top--];
    }
    return -1;
}

// Верхний элемент стека
int peek(Stack* stack)
{
    if (!isStackEmpty(stack))
    {
        return stack->data[stack->top];
    }
    return -1;
}

// Приоритет операторов
int priority(char op)
{
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

// Проверка на оператор
int isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Замена $ на значение memory
void replace_dollar_with_memory(char* str, int memory)
{
    char temp[MAX_INPUT + 1];
    int k = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '$')
        {
            // Memory в строку и копируем её
            char num_str[20];
            sprintf(num_str, "%d", memory);
            strcat(temp + k, num_str);
            k += strlen(num_str);
        }
        else
        {
            temp[k++] = str[i];
        }
    }
    temp[k] = '\0';
    strcpy(str, temp);
}

// inf -> post
int inf_to_post(const char* str, char* post)
{
    Stack stack;
    initStack(&stack);
    int k = 0;
    int waitOperator = 1;
    for (int i = 0; str[i] != '\0'; i++)
    {
        char c = str[i];
        if (isspace(c)) continue;
        if (isdigit(c))
        {
            while (isdigit(str[i]))
            {
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = str[i++];
            }
            if (k >= MAX_POSTFIX) return 0;
            post[k++] = ' ';
            i--;
            waitOperator = 0;
        }
        else if (c == '(')
        {
            push(&stack, c);
            waitOperator = 1;
        }
        else if (c == ')')
        {
            if (waitOperator) return 0;
            while (!isStackEmpty(&stack) && peek(&stack) != '(')
            {
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = pop(&stack);
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = ' ';
            }
            if (isStackEmpty(&stack)) return 0;
            pop(&stack);
        }
        else if (isOperator(c))
        {
            if (waitOperator) return 0;
            while (!isStackEmpty(&stack) &&
                (priority(peek(&stack)) > priority(c) ||
                    (priority(peek(&stack)) == priority(c) && c != '^')))
            {
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = pop(&stack);
                if (k >= MAX_POSTFIX) return 0;
                post[k++] = ' ';
            }
            push(&stack, c);
            waitOperator = 1;
        }
        else
        {
            return 0;
        }
    }
    if (waitOperator) return 0;
    while (!isStackEmpty(&stack))
    {
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

// Счет постфикса
int evaluate_post(const char* post, int* result)
{
    Stack stack;
    initStack(&stack);
    for (int i = 0; post[i] != '\0'; i++)
    {
        if (isspace(post[i])) continue;
        if (isdigit(post[i]))
        {
            int num = 0;
            while (isdigit(post[i]))
            {
                num = num * 10 + (post[i++] - '0');
            }
            push(&stack, num);
            i--;
        }
        else if (isOperator(post[i]))
        {
            if (stack.top < 1)
            {
                return 0;
            }
            int b = pop(&stack);
            int a = pop(&stack);
            switch (post[i])
            {
            case '+': push(&stack, a + b);
                break;
            case '-': push(&stack, a - b);
                break;
            case '*': push(&stack, a * b);
                break;
            case '/':
                if (b == 0) return -1;
                push(&stack, a / b);
                break;
            case '^':
                push(&stack, (int)pow(a, b));
                break;
            default: return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    if (stack.top != 0)
    {
        return 0;
    }
    *result = pop(&stack);
    return 1;
}

int main(int argc, char* argv[])
{
    char post[MAX_POSTFIX + 1];
    int result;

    // Проверяем количество параметров
    if (argc > 2)
    {
        printf("Too many parameters or incorrect usage.\n");
        printf("Usage: %s [expression] | %s -i\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "-i") == 0)
        {
            // Интерактивный режим
            printf("\nSend EXIT to STOP\n");

            int memory = 0;

            while (1)
            {
                printf("Enter your string: ");
                char str[MAX_INPUT + 1];

                if (!fgets(str, sizeof(str), stdin))
                {
                    printf("syntax error\n");
                    break;
                }
                // Заменяем символы $ на значение memory
                replace_dollar_with_memory(str, memory);

                str[strcspn(str, "\n")] = 0;

                if (strcmp(str, "EXIT") == 0)
                {
                    return 0;
                }

                if (!inf_to_post(str, post))
                {
                    printf("syntax error\n");
                    continue;
                }

                int mode = evaluate_post(post, &result);

                if (mode == -1)
                {
                    printf("division by zero\n");
                }
                else if (mode == 0)
                {
                    printf("syntax error\n");
                }
                else
                {
                    printf("%d\n", result);
                    memory = result;
                }
            }
        }
    }
    else
    {
        char str1[MAX_INPUT + 1];
        int result1;

        if (!fgets(str1, sizeof(str1), stdin))
        {
            printf("syntax error\n");
            return EXIT_SUCCESS;
        }

        if (!inf_to_post(str1, post))
        {
            printf("syntax error\n");
            return EXIT_SUCCESS;
        }

        int mode = evaluate_post(post, &result1);

        if (mode == -1)
        {
            printf("division by zero\n");
        }
        else if (mode == 0)
        {
            printf("syntax error\n");
        }
        else
        {
            printf("%d\n", result1);
        }

        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
