// Code ADT

#include "code.h"

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Initializes a code
Code code_init(void) {
    Code c;
    c.top = 0;
    //Zero out the bits array
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i = i + 1) {
        c.bits[i] = 0;
    }
    return c;
}

// Returns size of code
uint32_t code_size(Code *c) {
    return c->top;
}

// Returns if code is empty
bool code_empty(Code *c) {
    return c->top == 0;
}

// Returns if code is full;
bool code_full(Code *c) {
    return c->top == ALPHABET;
}

// Pushes a bit to the code, returns status of push
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }

    uint32_t location = c->top / 8;
    uint32_t position = c->top % 8;
    if (bit == 0) {
        // clear bit
        c->bits[location] = c->bits[location] & ~(1 << position);

    } else {
        //set bit
        c->bits[location] = c->bits[location] | (1 << position);
    }
    c->top = c->top + 1;
    return true;
}

// Pops a bit from the code, returns status of pop
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top = c->top - 1;
    // Get bit at index c->top
    uint32_t location = c->top / 8;
    uint32_t position = c->top % 8;
    *bit = ((1 << position) & c->bits[location]) >> position;
    return true;
}

// Prints code
void code_print(Code *c) {
    printf("[");
    for (uint32_t i = 0; i < c->top; i = i + 1) {
        uint32_t location = i / 8;
        uint32_t position = i % 8;
        uint8_t bit = ((1 << position) & c->bits[location]) >> position;
        printf("%d", bit);
        if (i + 1 != c->top) {
            printf(", ");
        }
    }
    printf("]\n");
}
