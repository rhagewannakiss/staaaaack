#ifndef STACK_H_
#define STACK_H_

#include  <stdio.h>
#include <stdlib.h>

typedef void void_sex;

typedef int Stack_t;

typedef struct Stack_struct {
    Stack_t*  data;
    size_t  nElems;
    int   capacity;
} Stack_struct;


typedef enum ERROR_CASE {
    EVERYTHING_IS_OK = 0,
    MEM_ALLOC_ERR = -1,
    STACK_UNDERFLOW = -2
} stack_error;

stack_error push_func(Stack_struct* stack, Stack_t new_elem);
stack_error   pop_func(Stack_struct* stack, Stack_t* top_elem);
void_sex Ctor_stack_func(Stack_struct* stack);
void_sex Dtor_stack_func(Stack_struct* stack);

const int default_capacity = 32;
const int ScaleFactor = 2;

#endif // STACK_H_