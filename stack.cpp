#include    <stdio.h>
#include   <stdlib.h>
#include   <assert.h>
#include   <string.h>
#include   <stdint.h>
#include    "stack.h"
#include     "hash.h"

static const char* poison_checker(stack_struct* stack, ssize_t i);
static void_sex    let_the_canaries_sing(stack_struct* stack);
static stack_error stack_realloc(stack_struct* stack);
static void_sex    hash_update(stack_struct* stack);
static void_sex    poison(stack_struct* stack);

//----------------------------------- CTOR ----------------------------------

stack_error Ctor_stack(stack_struct* stack) {
    if (stack == NULL) {
        stack->error_code = stack_error::NULL_PTR;
        return stack->error_code;
    }

    stack->canary_beginning = 0xB00BA;
    stack->number_of_elems = 0;
    stack->capacity = default_capacity + 2;
    stack->canary_end = 0xC0FFEE;
    stack->data = (stack_t*)calloc((size_t)stack->capacity, sizeof(stack_t));

    if (stack->data == NULL) {
        stack->error_code = stack_error::MEM_ALLOC_ERR;
        return stack->error_code;
    }

    let_the_canaries_sing(stack);

    hash_update(stack);

    stack->error_code = EVERYTHING_IS_OK;

    return stack->error_code;
}

//----------------------------------- DTOR ----------------------------------

stack_error Dtor_stack(stack_struct* stack) {
    stack_assert(stack);

    free(stack->data);

    stack->canary_beginning =          0;
    stack->data =                   NULL;
    stack->number_of_elems =           0;
    stack->capacity =                  0;
    stack->canary_end =                0;
    stack->error_code = EVERYTHING_IS_OK;
    stack->struct_hash =               0;
    stack->data_hash =                 0;

    return stack->error_code;
}

//--------------------------------- PUSH -------------------------------------

stack_error stack_push(stack_struct* stack, stack_t new_elem) {
    stack_assert(stack);

    if (stack->number_of_elems + 1 == stack->capacity - 2) {
        stack->capacity *= scale_factor;
        stack_realloc(stack);
    }

    stack->data[stack->number_of_elems + 1] = new_elem;
    stack->number_of_elems++;

    hash_update(stack);

    poison(stack);

    stack_assert(stack);
    return EVERYTHING_IS_OK;
}

//--------------------------------- POP --------------------------------------

stack_error stack_pop(stack_struct* stack, stack_t* top_elem) {
    stack_assert(stack);

    if (stack->number_of_elems <= 0) {
        stack->error_code = stack_error::ZERO_ELEMS;
        return stack->error_code;
    }

    if (top_elem == NULL) {
        stack->error_code = stack_error::EMPTY_TOP_ELEM;
        return stack->error_code;
    }

    fprintf(stderr, "n of elems %ld\n", stack->number_of_elems);

    *top_elem = (stack->data)[stack->number_of_elems - 1];

    if (stack->number_of_elems - 1 == (stack->capacity / (2 * scale_factor))) {
        stack->capacity = stack->capacity > default_capacity
                            ? stack->capacity / scale_factor
                            : default_capacity + 2;
        stack_realloc(stack);
    }

    stack->number_of_elems--;

    let_the_canaries_sing(stack);

    hash_update(stack);

    poison(stack);

    return stack->error_code;
}

//----------------------------------- ASSERT FUNCTION -----------------------------

stack_error stack_assert(stack_struct* stack) {
    stack->error_code = EVERYTHING_IS_OK;

    if (stack->data == NULL) {
        stack->error_code = stack_error::MEM_ALLOC_ERR;
        dump(stack);
        abort();
    }

    if  ((stack->data)[0] != 0xCA3AC) {
        stack->error_code = stack_error::DEAD_LEFT_ARR_CANARY;
        dump(stack);
        abort();
    }

    if ((stack->data)[stack->capacity - 1] != 0xBA51C) {
        stack->error_code = stack_error::DEAD_RIGHT_ARR_CANARY;
        dump(stack);
        abort();
    }

    if (stack->canary_beginning != 0xB00BA) {
        stack->error_code = stack_error::DEAD_LEFT_STRUCT_CANARY;
        dump(stack);
        abort();
    }

    if (stack->canary_end != 0xC0FFEE) {
        stack->error_code = stack_error::DEAD_RIGHT_STRUCT_CANARY;
        dump(stack);
        abort();
    }

    if (stack->capacity <= 0) {
        stack->error_code = stack_error::WRONG_CAPACITY;
        dump(stack);
        abort();
    }

    if (stack->number_of_elems > stack->capacity - 2) {
        stack->error_code = stack_error::STACK_OVERFLOW;
        dump(stack);
        abort();
    }

    if (stack->number_of_elems < 0) {
        stack->error_code = stack_error::STACK_UNDERFLOW;
        dump(stack);
        abort();
    }

    uint32_t    given_data_hash = stack->data_hash;
    uint32_t expected_data_hash = murmurhash_32((const uint8_t*)stack->data, (size_t)stack->number_of_elems * sizeof(stack->data[0]));

    if (given_data_hash != expected_data_hash) {
        stack->error_code = stack_error::WEIRD_DATA_HASH;
        fprintf(stderr, "hash diff: %u \n", expected_data_hash - stack->data_hash);
        fprintf(stderr, "expected data hash: %u \n", expected_data_hash);
        fprintf(stderr, "given data hash: %u \n", stack->data_hash);
        dump(stack);
        abort();
    }

    uint32_t given_struct_hash = stack->struct_hash;
    stack->struct_hash = 0;
    uint32_t expected_struct_hash = murmurhash_32((const uint8_t*)stack, sizeof(stack));

    if (given_struct_hash != expected_struct_hash) {
        stack->error_code = stack_error::WEIRD_STRUCT_HASH;
        fprintf(stderr, "hash diff: %u \n", expected_struct_hash - stack->struct_hash);
        fprintf(stderr, "expected struct hash:  %u \n", expected_struct_hash);
        fprintf(stderr, "given struct hash: %u \n", stack->struct_hash);
        dump(stack);
        abort();
    }
    stack->struct_hash = given_struct_hash;
    return (stack->error_code);
}

//------------------------------------ DUMP FUNCTION -------------------------------

void_sex dump(stack_struct* stack) {
    fprintf(stderr, "Stack_condition - %s \n", stack_error_to_str(stack->error_code));
    fprintf(stderr, "Data hash: %u  \n", stack->data_hash);
    fprintf(stderr, "Stack hash: %u  \n", stack->struct_hash);
    fprintf(stderr, "Left canary in struct: 0x%lX  \n", stack->canary_beginning);
    fprintf(stderr, "Right canary in struct: 0x%lX \n", stack->canary_end);
    fprintf(stderr, "number of elements: %zd \n", stack->number_of_elems);
    fprintf(stderr, "capacity: %zd \n\n", stack->capacity - 2);
    fprintf(stderr, "data:  {\n");

    fprintf(stderr, "Left canary = 0x%lX \n", (stack->data)[0]);
    for (ssize_t i = 1; i < (stack->capacity - 1); i++) {
        fprintf(stderr, "\t[%ld] = %lu %s \n", i, (stack->data)[i], poison_checker(stack, i));

    }
    fprintf(stderr, "Right canary = 0x%lX \n", (stack->data)[stack->capacity - 1]);

    fprintf(stderr, "}\n\n");
}

//---------------------------------- POISON FUNCTION ---------------------------------

static void_sex poison(stack_struct* stack) {
    stack_assert(stack);

    for (int i = 1; (stack->number_of_elems) + i < (stack->capacity - 1); i++) {
        (stack->data)[(stack->number_of_elems) + i] = 0xC0CA14E;
    }
}

//---------------------------------- STACK REALLOC ------------------------------------

static stack_error stack_realloc(stack_struct* stack) {
    assert(stack != NULL);

    stack->data = (stack_t*)realloc(stack->data, (size_t)(stack->capacity + 2) * sizeof(stack_t));
    //FIXME подумаьб  над тем что будет если stack_t бужет большего размера
    if (stack == NULL) {
        return stack_error::MEM_ALLOC_ERR;
        return stack->error_code;
    }
    let_the_canaries_sing(stack);

    return EVERYTHING_IS_OK;
}

//--------------------------------------- CANARY --------------------------------------

static void_sex let_the_canaries_sing(stack_struct* stack) {
    assert(stack != NULL);

    (stack->data)[0] = canary_data_beggining;
    (stack->data)[stack->capacity - 1] = canary_data_end;
}

//---------------------------------- STACK ERROR TO STR --------------------------------

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
        case WEIRD_STRUCT_HASH:                            return TOSTR(WEIRD_STRUCT_HASH);
        case WEIRD_DATA_HASH:                              return TOSTR(WEIRD_DATA_HASH);
        case ZERO_ELEMS:                                   return TOSTR(ZERO_ELEMS);

        default: assert(0 && "invalid stack_error value"); return "<invalid stack_error value>";
    }
}

//-------------------------------------- POISON CHECKER -----------------------------------

static const char* poison_checker(stack_struct* stack, ssize_t i) {
    if (((stack->data)[i]) == 0xC0CA14E) {
        return "   POISON!";
    }
    else return "";
}

//--------------------------------------- HASH UPDATE -------------------------------------

static void_sex hash_update(stack_struct* stack) {
    stack->struct_hash = 0;
    stack->data_hash =   0;
    stack->data_hash =   murmurhash_32((const uint8_t*)stack->data, (size_t)stack->number_of_elems * sizeof(stack->data[0]));
    stack->struct_hash = murmurhash_32((const uint8_t*)stack, sizeof(stack));
}

//-------------------------------------- STRING CONVERTER ---------------------------------

// написать функцию которая будет принимать стак элем т и распечатывает его или оформляет
// в строку и передавать в дамп указатель на функцию
//char* string_converter(stack_struct* stack) {

    // size_t size_of_arg = ;

    // char* unconverted_str = (char*)malloc(size_of_arg * 2 + 1);

    // if (unconverted_str == NULL) {
    //     return NULL;
    // }

    // switch (size_of_arg) {
    //     case sizeof(char):
    //         sprintf(unconverted_str, "%c", *(char*)ptr);
    //         break;
    //     case sizeof(short):
    //         sprintf(unconverted_str, "%hd", *(short*)ptr);
    //         break;
    //     case sizeof(int):
    //         sprintf(unconverted_str, "%d", *(int*)ptr);
    //         break;
    //     case sizeof(long):
    //         sprintf(unconverted_str, "%ld", *(long*)ptr);
    //         break;
    //     case sizeof(float):
    //         sprintf(unconverted_str, "%f", *(float*)ptr);
    //         break;
    //     case sizeof(double):
    //         sprintf(unconverted_str, "%f", *(double*)ptr);
    //         break;
    //     case sizeof(long double):
    //         sprintf(unconverted_str, "%Lf", *(long double*)ptr);
    //         break;
    //     default: ________;
    // }

    // return unconverted_str;
    //}