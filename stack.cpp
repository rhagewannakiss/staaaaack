#include  <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "stack.h"

//----------------------------------- CTOR ----------------------------------

void_sex Ctor_stack_func(Stack_struct* stack) {
    assert(stack != NULL);

    stack->data = (Stack_t*)calloc(default_capacity, sizeof(Stack_t));
    stack->nElems =   0;
    stack->capacity = default_capacity;
}

//----------------------------------- DTOR ----------------------------------

void_sex Dtor_stack_func(Stack_struct* stack) { //и тут везде будут всякие штуки наподобие ассертов еще по идее да
    assert(stack != NULL);
    assert(stack->data != NULL);

    if (stack->data !=  NULL) {
        free(stack->data);
        stack->data = NULL;
    }

    stack->nElems = 0;
    stack->capacity = default_capacity;
}

//--------------------------------- PUSH -------------------------------------

stack_error push_func(Stack_struct* stack, Stack_t new_elem) { // asserts
    assert(stack != NULL);
    assert(stack->data != NULL);

    if (stack->nElems + 1 == stack->capacity) {
        stack->capacity = stack->capacity * ScaleFactor;
        stack->data = (Stack_t*)realloc(stack->data, stack->capacity * sizeof(Stack_t)); // потом проверку ещё
    }

    if (stack->data == NULL) {
        return MEM_ALLOC_ERR;
    }

    stack->data[stack->nElems] = new_elem;
    stack->nElems++;

    return EVERYTHING_IS_OK;
}

//--------------------------------- POP --------------------------------------

stack_error pop_func(Stack_struct* stack, Stack_t* top_elem) {
    assert(stack != NULL);

    if (stack->nElems == 0) {
        return STACK_UNDERFLOW;
    }

    *top_elem = (stack->data)[stack->nElems - 1];

    stack->data = (Stack_t*)realloc(stack->data, (stack->nElems - 1) * sizeof(Stack_t));

    if (stack->data == NULL) {
        return MEM_ALLOC_ERR;
    }

    stack->nElems--;
    return EVERYTHING_IS_OK;
}

//c++ top func