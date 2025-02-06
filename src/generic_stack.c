//IMPORTANT NOTE FOR GRADER:\
//The original implementation of this data structure \
//was done for only int type, using the help of an article.\
//I have included the original implementation in the stack.c and stack.h files \
//and the article that was used as a reference.\
//\
//The final implementation accepts generic types and is implemented using macros\
//This was done with the help of a different article:\
//https://iafisher.com/blog/2020/06/type-safe-generics-in-c\

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define DECL_STACK(typename, type)\
typedef struct typename{\
  size_t maxSize;\
  type* array;\
  int top;\
} typename;\
\
typename* typename##_create(size_t maxSize);\
void typename##_free(typename* stack);\
bool typename##_isFull(typename* stack);\
bool typename##_isEmpty(typename* stack);\
void typename##_push(typename* stack, type value);\
void typename##_pop(typename* stack, type* retVal);\
bool typename##_peek(typename* stack, type* retVal);\
\
typename* typename##_create(size_t max){\
  typename* stack = (typename*)malloc(sizeof(typename));\
  stack->maxSize = max;\
  stack->array = (type*)calloc(max, sizeof(type));\
  stack->top = -1;\
  return stack;\
}\
\
void typename##_free(typename* stack){\
  if(stack == NULL)\
    return;\
  free(stack->array);\
  free(stack);\
}\
\
bool typename##_isFull(typename* stack){\
  /*THIS ONE LINE SENT ME ON A WILD GOOSE CHASE TO FIND OUT WHY MY SEEMINGLY SOUND LOGIC WAS NOT WORKING\
   * AND IT TURNS OUT I HAD TO JUST CAST THE SIZE_T INTO AN INT FOR SOME REASON????\
   * even tho i cant think of reason it shouldnt since size_t is just an unsigned int so ??????*/\
  return stack->top >= (int)stack->maxSize - 1;\
}\
\
bool typename##_isEmpty(typename* stack){\
  return stack->top <= -1;\
}\
\
void typename##_push(typename* stack, type value){\
  if (typename##_isFull(stack)){\
    printf("This stack is full! Top: %d\n", stack->top);\
    return;\
  }\
  stack->top++;\
  stack->array[stack->top] = value;\
}\
\
/*\
the reason i use a paramter for the return value\
is because of error handling, its hard to return an error value directly\
so i will simply not return a value\
*/\
\
void typename##_pop(typename* stack, type* retVal){\
  if(typename##_isEmpty(stack)){\
    printf("This stack is empty!\n");\
    return;\
  }\
  *retVal = stack->array[stack->top];\
  stack->top--;\
}\
\
bool typename##_peek(typename* stack, type* retVal){\
  if(!typename##_isEmpty(stack)){\
    *retVal = stack->array[stack->top];\
    return true;\
  }\
  printf("Could not peek into stack, it is empty.\n");\
  return false;\
}\

