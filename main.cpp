#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <search.h>
#include "stack.h"


//--------------------------------- MAIN -------------------------------------

int main(/*int argc, char* argv[] или не нужно*/) {
    Stack_struct stk = {};
    Ctor_stack_func(&stk);

    push_func(&stk, 'a');

    fprintf(stderr, "Num of elems: %d \n", stk.nElems);
    fprintf(stderr, "Stack after push: [");
    for (size_t i = 0; i < stk.nElems; i++) {
        fprintf(stderr, " %c", (stk.data)[i]);
    }
    fprintf(stderr, "]\n\n");

    Stack_t top_elem = 0;
    pop_func(&stk, &top_elem);
    fprintf(stderr, "Popped element: %c \n", top_elem);

    fprintf(stderr, "Num of elems: %d\n", stk.nElems);
    fprintf(stderr, "Stack after pop: [");
    for (size_t i = 0; i < stk.nElems; i++) {
        fprintf(stderr, " %c", (stk.data)[i]);
    }
    fprintf(stderr, "]\n\n");

    Dtor_stack_func(&stk);
    return 0;
}

