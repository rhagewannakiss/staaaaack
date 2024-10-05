#include  <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include  "stack.h"
#include   "hash.h"

static void_sex let_the_canaries_sing(Stack_struct* stack);
static void_sex poison(Stack_struct* stack);
static void_sex dump(Stack_struct* stack);

//switch for enum ???

//----------------------------------- CTOR ----------------------------------

stack_error Ctor_stack(Stack_struct* stack) {
    if (stack == NULL) {
        stack->error_code = stack_error::NULL_PTR;
    }

    stack->canary_beginning = 0xB00BA;
    stack->number_of_elems = 0;
    stack->capacity = default_capacity;
    stack->canary_end = 0xC0FFEE;
    stack->data = (Stack_t*)calloc(default_capacity, sizeof(Stack_t));
    if (stack->data == NULL) {
        stack->error_code = stack_error::MEM_ALLOC_ERR;
    }

    return stack->error_code;
}

//----------------------------------- DTOR ----------------------------------

stack_error Dtor_stack(Stack_struct* stack) {
    stack_assert(stack);

    free(stack->data);
    stack->canary_beginning = 0;
    stack->data = NULL;
    stack->number_of_elems = 0;
    stack->capacity = 0;
    stack->canary_end = 0;
    stack->error_code = EVERYTHING_IS_OK;

    return EVERYTHING_IS_OK;
}

//--------------------------------- PUSH -------------------------------------

stack_error stack_push(Stack_struct* stack, Stack_t new_elem) {
    stack_assert(stack);

    if (stack->number_of_elems + 1 == stack->capacity) {
        stack->capacity = stack->capacity * scale_factor;
        stack_realloc(stack);
    }

    //чтобы хакеры сосали перещаписываем канареечку десятичной или двоичным и записывать ее вообще по алресу

    stack->data[stack->number_of_elems] = new_elem;
    stack->number_of_elems++;
    stack_assert(stack);

    poison(stack);

    stack->data_hash = murmurhash_32((const uint8_t*)stack->data, stack->number_of_elems);

    return EVERYTHING_IS_OK;
}

//--------------------------------- POP --------------------------------------

stack_error stack_pop(Stack_struct* stack, Stack_t* top_elem) {
    stack_assert(stack);

    if (top_elem == NULL) {
        return stack_error::EMPTY_TOP_ELEM;
    }

    *top_elem = (stack->data)[stack->number_of_elems - 1];

    if (stack->data == NULL) {
        return stack_error::MEM_ALLOC_ERR;
    }

    if ((stack->number_of_elems - 1) == (stack->capacity / (2*scale_factor))) {
        stack->capacity = stack->capacity / scale_factor;
        stack_realloc(stack);
    }

    size_t canary_data_beggining = 0xCA3AC;
    (stack->data)[0] = canary_data_beggining;

    stack->number_of_elems--;
    poison(stack);

    stack->data_hash = murmurhash_32((const uint8_t*)stack->data, stack->number_of_elems);

    return EVERYTHING_IS_OK;
}

//----------------------------------- ASSERT FUNCTION -----------------------------

stack_error stack_assert(Stack_struct* stack) {
    stack->error_code = EVERYTHING_IS_OK;

    if (stack->data == NULL) {
        dump(stack);
        stack->error_code = stack_error::MEM_ALLOC_ERR;
        abort();
        stack->error_code = stack_error::MEM_ALLOC_ERR;
    }

    if  ((stack->data)[0] != 0xCA3AC) {
        dump(stack);
        abort();
        stack->error_code = stack_error::DEAD_LEFT_ARR_CANARY;
    }

    if ((stack->data)[stack->capacity + 1]) {
        dump(stack);
        abort();
        stack->error_code = stack_error::DEAD_RIGHT_ARR_CANARY;
    }

    if (stack->canary_beginning != 0xB00BA) {
        dump(stack);
        abort();
        stack->error_code = stack_error::DEAD_LEFT_STRUCT_CANARY;
    }

     if (stack->canary_end != 0xC0FFEE) {
        dump(stack);
        abort();
        stack->error_code = stack_error::DEAD_RIGHT_STRUCT_CANARY;
    }

     if (stack->capacity >= 0) {
        dump(stack);
        abort();
        stack->error_code = stack_error::WRONG_CAPACITY;
    }

    if (stack->number_of_elems > stack->capacity + 2) {
        dump(stack);
        abort();
        stack->error_code = stack_error::STACK_OVERFLOW;
    }

    if (stack->number_of_elems < 0) {
        dump(stack);
        abort();
       stack->error_code = stack_error::STACK_UNDERFLOW;
    }

    return (stack->error_code);
}

//------------------------------------ DUMP FUNCTION -------------------------------

static void_sex dump(Stack_struct* stack) {
    fprintf(stderr, "Stack_condition - %s \n", stack_error_to_str(stack->error_code));
    fprintf(stderr, "Data hash: %zd  \n", stack->data_hash);
    fprintf(stderr, "Stack hash: %zd  \n", stack->struct_hash);
    fprintf(stderr, "Left canary in struct: %lX  \n", stack->canary_beginning);
    fprintf(stderr, "Right canary in struct: %lX \n", stack->canary_end);
    fprintf(stderr, "number of elements: %zd \n", stack->number_of_elems);
    fprintf(stderr, "capacity: %zd \n\n", stack->capacity);
    fprintf(stderr, "data:  {\n");
    // добавить выравнивание и прочую шнягу
    //передавать макросами строчку и файл

    fprintf(stderr, "Left canary = %lX \n", (stack->data)[0]);
    for (int i = 1; i < stack->number_of_elems; i++) {
        fprintf(stderr, "*[%d] = %lu \n", i, (stack->data)[i]);
    }
    for (ssize_t i = stack->number_of_elems; i < stack->capacity + 1; i++) {
        fprintf(stderr, "[%ld] = %lu  POISON! \n", i, (stack->data)[i]);
    }
    fprintf(stderr, "Right canary = %lX \n", (stack->data)[(stack->capacity + 1)]);

    fprintf(stderr, "}\n\n");
}

//---------------------------------- POISON FUNCTION -------------------------------

static void_sex poison(Stack_struct* stack) {
    stack_assert(stack);

    for (int i = 0; (stack->number_of_elems) + i < (stack->capacity + 2); i++) {
        (stack->data)[(stack->number_of_elems) + i] = 0xC0CA14E;
    }
}

//------------------------------------ PRINTF STACK ---------------------------------

void_sex printf_stack(Stack_struct* stack) {
    fprintf(stderr, "Num of elems: %ld \n", stack->number_of_elems);

    fprintf(stderr, "Stack: [");
    for (ssize_t i = 0; i < stack->number_of_elems; i++) {
        fprintf(stderr, " %lu", (stack->data)[i]);
    }
    fprintf(stderr, "]\n\n");
}

//---------------------------------- STACK REALLOC ------------------------------------

stack_error stack_realloc(Stack_struct* stack) {
    stack_assert(stack);

    stack->data = (Stack_t*)realloc(stack->data, (stack->capacity + 2) * sizeof(Stack_t)); //FIXME подумаьб  над тем что будет если Stack_t бужет большего размера
    if (stack == NULL) {
        return stack_error::MEM_ALLOC_ERR;
    }
    let_the_canaries_sing(stack);

    return EVERYTHING_IS_OK;
}

//--------------------------------------- CANARY ---------------------------------------

static void_sex let_the_canaries_sing(Stack_struct* stack) {
    size_t canary_data_beggining = 0xCA3AC;
    (stack->data)[0] = canary_data_beggining;

    size_t canary_data_end = 0xBA51C;
    (stack->data)[stack->capacity + 2] = canary_data_end;

}

//---------------------------------- STACK ERROR TO STR ---------------------------------

const char* stack_error_to_str(stack_error error) {
    switch (error) {
        case EVERYTHING_IS_OK:                             return TOSTR(EVERYTHING_IS_OK);
        case MEM_ALLOC_ERR:                                return TOSTR(MEM_ALLOC_ERR);
        case STACK_UNDERFLOW:                              return TOSTR(STACK_UNDERFLOW);
        case DEAD_LEFT_ARR_CANARY:                         return TOSTR(DEAD_LEFT_ARR_CANARY);
        case DEAD_RIGHT_ARR_CANARY:                        return TOSTR(DEAD_RIGHT_ARR_CANARY);
        case STACK_OVERFLOW:                               return TOSTR(STACK_OVERFLOW);
        case WRONG_CAPACITY:                               return TOSTR(WRONG_CAPACITY);
        case DEAD_LEFT_STRUCT_CANARY:                      return TOSTR(DEAD_LEFT_STRUCT_CANARY);
        case DEAD_RIGHT_STRUCT_CANARY:                     return TOSTR(DEAD_RIGHT_STRUCT_CANARY);
        case EMPTY_TOP_ELEM:                               return TOSTR(EMPTY_TOP_ELEM);
        case NULL_PTR:                                     return TOSTR(NULL_PTR);
        default: assert(0 && "invalid stack_error value"); return "<invalid stack_error value>";
    }
}