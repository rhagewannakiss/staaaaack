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

    stack_push(&stk, 'a');

    fprintf(stderr, "Num of elems: %ld \n", stk.number_of_elems);
    dump(&stk);


    stack_t top_elem = 0;
    stack_pop(&stk, &top_elem);
    fprintf(stderr, "Popped element: %lu \n", top_elem);
    dump(&stk);

    Dtor_stack(&stk);
    return 0;
}

