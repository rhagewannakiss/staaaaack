#ifndef STACK_H_
#define STACK_H_

#include  <stdio.h>
#include <stdlib.h>

#define TOSTR(x) #x

typedef void void_sex;
typedef size_t Stack_t;

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
} stack_error;

typedef struct Stack_struct {
    size_t canary_beginning;
    Stack_t*           data;
    ssize_t number_of_elems;
    ssize_t        capacity;
    ssize_t       data_hash;
    ssize_t     struct_hash;
    size_t       canary_end;
    stack_error  error_code;
} Stack_struct;

// typedef enum PASSWORD_VERIFICATION_STATUS {
//     PASSWORD_VERIFICATED = 1,
//     WRONG_PASSWORD       = 0,
//     //хз пока что нужно ли это вообще можно удалить будет потом
// };

stack_error stack_push(Stack_struct* stack, Stack_t new_elem);
stack_error stack_pop(Stack_struct* stack, Stack_t* top_elem);
const char* stack_error_to_str(stack_error error);
stack_error stack_realloc(Stack_struct* stack);
stack_error stack_assert(Stack_struct* stack);
void_sex    printf_stack(Stack_struct* stack);
stack_error Ctor_stack(Stack_struct* stack);
stack_error Dtor_stack(Stack_struct* stack);

const int default_capacity = 32;
const int scale_factor = 2;



#endif // STACK_H_

