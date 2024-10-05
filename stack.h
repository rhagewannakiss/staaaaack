#ifndef STACK_H_
#define STACK_H_

#include  <stdio.h>
#include <stdlib.h>

#define TOSTR(x) #x

typedef void void_sex;
typedef size_t stack_t;

typedef enum ERROR_CASE {
    EVERYTHING_IS_OK =         -1,
    MEM_ALLOC_ERR =             1,
    STACK_UNDERFLOW =           2,
    DEAD_LEFT_ARR_CANARY =      3,
    DEAD_RIGHT_ARR_CANARY =     4,
    STACK_OVERFLOW =            5,
    WRONG_CAPACITY =            6,
    DEAD_LEFT_STRUCT_CANARY =   7,
    DEAD_RIGHT_STRUCT_CANARY =  8,
    EMPTY_TOP_ELEM =            9,
    NULL_PTR =                  10,
    WEIRD_STRUCT_HASH =         11,
    WEIRD_DATA_HASH =           12,
    ZERO_ELEMS =                13
} stack_error;

typedef struct stack_struct {
    size_t canary_beginning;
    stack_t*           data;
    ssize_t number_of_elems;
    ssize_t        capacity;
    uint32_t      data_hash;
    uint32_t    struct_hash;
    size_t       canary_end;
    stack_error  error_code;
} stack_struct;

stack_error stack_push(stack_struct* stack, stack_t new_elem);
stack_error stack_pop(stack_struct* stack, stack_t* top_elem);
const char* stack_error_to_str(stack_error error);
stack_error stack_assert(stack_struct* stack);
stack_error Ctor_stack(stack_struct* stack);
stack_error Dtor_stack(stack_struct* stack);
void_sex    dump(stack_struct* stack);

static const int default_capacity = 32;
static const int scale_factor = 2;

static const size_t canary_data_beggining = 0xCA3AC;
static const size_t canary_data_end = 0xBA51C;

#endif // STACK_H_

