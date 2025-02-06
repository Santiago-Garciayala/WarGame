#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct Stack{
  int maxSize;
  int* array;
  int top;
} Stack;

Stack createStack(int maxSize);
void freeStack(Stack* stack);
bool isFull(Stack* stack);
bool isEmpty(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);

#endif //!STACK_H
