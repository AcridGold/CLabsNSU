#ifndef Calc_H
#define Calc_H

#define MAX_INPUT 1024
#define MAX_POSTFIX (2 * MAX_INPUT)

// ~~~Int-tipe~~~

// Структура стека
typedef struct Stack {
    int data[MAX_INPUT];
    int top;
} Stack;

// Функции стека
void initStack(Stack* stack);
int isStackEmpty(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);
int peek(Stack* stack);

// Функции калькулятора
int priority(char op);
int isOperator(char c);
int inf_to_post(const char* str, char* post);
int evaluate_post(const char* post, int* result);

// ~~~Float-tipe~~~

// Определение стека для чисел с плавающей точкой
typedef struct {
    double data[MAX_INPUT];
    int top;
} FloatStack;

// Функции для работы с FloatStack
void initFloatStack(FloatStack* stack);
int isFloatStackEmpty(FloatStack* stack);
void pushFloat(FloatStack* stack, double value);
double popFloat(FloatStack* stack);

// Функции калькулятора для работы с числами с плавающей точкой
void replace_dollar_with_memory(char* str, double memory);
int inf_to_post_float(const char* str, char* post);
int evaluate_post_float(const char* post, double* result);

#endif
