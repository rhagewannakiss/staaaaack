#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <search.h>
#include <stdint.h>
#include  "stack.h"
#include   "hash.h"


//--------------------------------- MAIN -------------------------------------

int main(/*int argc, char* argv[] */) {



    Stack_struct stk = {};
    Ctor_stack(&stk);

    stack_push(&stk, 'a'); // идея: сделать пуш на несколько элементов одновременно ??? ну и поп можно тоже

    fprintf(stderr, "Num of elems: %ld \n", stk.number_of_elems);
    printf_stack(&stk);


    Stack_t top_elem = 0;
    stack_pop(&stk, &top_elem);
    fprintf(stderr, "Popped element: %c \n", top_elem);

    printf_stack(&stk);

    Dtor_stack(&stk);
    return 0;
}

