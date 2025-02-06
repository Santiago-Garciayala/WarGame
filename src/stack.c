//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//ORIGNAL IMPLEMENTATION FOR INT TYPE

//this was written with the help of this article:
//https://www.geeksforgeeks.org/implement-stack-in-c/

#include "stack.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

//uses dynamic memory allocation, need to call freeStack when done using the stack
Stack createStack(int maxSize){
  Stack stack;
  stack.maxSize = maxSize;
  stack.array = (int*)malloc(sizeof(int) * maxSize);
  stack.top = -1;
  return stack;
}

void freeStack(Stack* stack){
  if(stack == NULL)
    return;
  free(stack->array);
  stack->maxSize = 0;
  stack->top = -1;
}

bool isFull(Stack* stack){
  if(stack->top > stack->maxSize - 1)
    return true;
  return false;
}

bool isEmpty(Stack* stack){
  if(stack->top <= -1)
    return true;
  return false;
}

void push(Stack* stack, int value){
  if(isFull(stack)){
    printf("This stack is full!\n");
    return;
  }
  stack->top++;
  stack->array[stack->top] = value;
}

int pop(Stack* stack){
  if(isEmpty(stack)){
    printf("This stack is empty!\n");
    return -1;
  }
  int retVal = stack->array[stack->top];
  stack->top--;
  return retVal;
}
