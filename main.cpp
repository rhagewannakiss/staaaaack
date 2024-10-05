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
    stack_struct stk = {};
    Ctor_stack(&stk);

    for (int i = 0; i < 200; i++) {
        stack_push(&stk, 'a');
    }

    dump(&stk);


    stack_t top_elem = 0;

    for (int i = 0; i < 200; i++) {
        stack_pop(&stk, &top_elem);
    }

    fprintf(stderr, "Popped element: %lu \n", top_elem);
    dump(&stk);

    Dtor_stack(&stk);
    return 0;
}

