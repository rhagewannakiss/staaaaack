#include  <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include  "stack.h"
#include   "hash.h"

//switch for enum

//----------------------------------- CTOR ----------------------------------

stack_error Ctor_stack(Stack_struct* stack) {
    if (stack == NULL) {
        return stack_error::NULL_PTR;
    }

    stack->canary_beginning = 0xB00BA;
    stack->number_of_elems = 0;
    stack->capacity = default_capacity;
    stack->canary_end = 0xC0FFEE;
    stack->data = (Stack_t*)calloc(default_capacity, sizeof(Stack_t));
    if (stack->data == NULL) {
        return stack_error::MEM_ALLOC_ERR;
    }

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
    stack->stack_error = 0;
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

    return EVERYTHING_IS_OK;
}

//--------------------------------- POP --------------------------------------

stack_error stack_pop(Stack_struct* stack, Stack_t* top_elem) {
    stack_assert(stack);

    if (top_elem == NULL) {
        return stack_error::EMPTY_TOP_ELEM;
    }

    *top_elem = (stack->data)[stack->number_of_elems - 1];
    fprintf(stderr, "top elem %c \n", top_elem);

    stack->data = (Stack_t*)realloc(stack->data, (stack->number_of_elems - 1) * sizeof(Stack_t));
    if (stack->data == NULL) {
        return stack_error::MEM_ALLOC_ERR;  //реалочить не на каждый элемент а на кусок памяти сразу и потом херню с гистерезисом
        //реалокать вниз от четверти capacity
    }

    size_t canary_data_beggining = 0xCA3AC;
    (stack->data)[0] = canary_data_beggining;



    // if ((int)(stack->number_of_elems - 1) == (stack->capacity / scale_factor)) {
    //     stack->capacity = stack->capacity / scale_factor;
    //     stack_realloc(stack);
    // }

    stack->number_of_elems--;
    poison(stack);

    return EVERYTHING_IS_OK;
}

//----------------------------------- ASSERT FUNCTION -----------------------------

stack_error stack_assert(Stack_struct* stack) {
    if (stack->data == NULL) {
        dump(stack);
        stack->stack_error = stack_error::MEM_ALLOC_ERR;
        abort();
        return stack_error::MEM_ALLOC_ERR;  //
    }

    if  ((stack->data)[0] != 0xCA3AC) {
        dump(stack);
        abort();
        return stack_error::DEAD_LEFT_ARR_CANARY;
    }

    if ((stack->data)[stack->capacity + 1]) {
        dump(stack);
        abort();
        return stack_error::DEAD_RIGHT_ARR_CANARY;
    }

    if (stack->canary_beginning != 0xB00BA) {
        dump(stack);
        abort();
        return stack_error::DEAD_LEFT_STRUCT_CANARY;
    }

     if (stack->canary_end != 0xC0FFEE) {
        dump(stack);
        abort();
        return stack_error::DEAD_RIGHT_STRUCT_CANARY;
    }

     if (stack->capacity >= 0) {
        dump(stack);
        abort();
        return stack_error::WRONG_CAPACITY;
    }

    if (stack->number_of_elems > stack->capacity + 2) {
        dump(stack);
        abort();
        return stack_error::STACK_OVERFLOW;
    }

    if (stack->number_of_elems < 0) {
        dump(stack);
        abort();
        return stack_error::STACK_UNDERFLOW;
    }
}

//------------------------------------ DUMP FUNCTION -------------------------------

static void_sex dump(Stack_struct* stack) { // FIXME stderr
    printf("Stack_condition -  \n");  //вот cюда enum-овую шнягу вывпдмть

    printf("Data hash ____  \n");
    printf("Stack hash ____  \n");
    printf("Left canary in struct: %H  \n", stack->canary_beginning);
    printf("Right canary in struct: %H \n", stack->canary_end);
    printf("size: %ld \n", stack->number_of_elems);
    printf("capacity: %ld \n", stack->capacity);
    printf("data:  {\n");
    // добавить выравнивание и прочую шнягу
    //передавать макросами строчку и файл

    printf("Left canary = %H \n", (stack->data)[0]);
    for (int i = 1; i < stack->number_of_elems; i++) {
        printf("*[%d] = %ld \n", i, (stack->data)[i]);
    }
    for (int i = stack->number_of_elems; i < stack->capacity + 1; i++) {
        printf("[%d] = %ld  POISON! \n", i, (stack->data)[i]);
    }
    printf("Right canary = %H \n", (stack->data)[(stack->capacity + 1)]);

    printf("}\n\n");
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
    fprintf(stderr, "Num of elems: %lu \n", stack->number_of_elems);

    fprintf(stderr, "Stack: [");
    for (size_t i = 0; i < stack->number_of_elems; i++) {
        fprintf(stderr, " %c", (stack->data)[i]);
    }
    fprintf(stderr, "]\n\n");
}

//---------------------------------- STACK REALLOC ------------------------------------

stack_error stack_realloc(Stack_struct* stack) {
    // assert
    stack->data = (Stack_t*)realloc(stack->data, (stack->capacity + 2) * sizeof(Stack_t)); //FIXME подумаьб  над тем что будет если Stack_t бужет большешго размера
    if (stack == NULL) {
        return stack_error::MEM_ALLOC_ERR;
    }
    let_the_canaries_sing(stack);
    // + испольхование хэшей
}

//--------------------------------------- CANARY ---------------------------------------

void_sex let_the_canaries_sing(Stack_struct* stack) {
    size_t canary_data_beggining = 0xCA3AC;
    (stack->data)[0] = canary_data_beggining;

    size_t canary_data_end = 0xBA51C;
    (stack->data)[stack->capacity + 2] = canary_data_end;
}