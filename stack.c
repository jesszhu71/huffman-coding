// Stack ADT

// MOST OF THIIS IS FROM PREVIOUS ASSIGNMENTS
#include "stack.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Stack struct ADT
struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

// Creates stack of capacity size
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity, sizeof(Node *));
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}

// Deletes/frees the stack in memory
void stack_delete(Stack **s) {
    if (*s && (*s)->items) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

// Returns if stack is empty
bool stack_empty(Stack *s) {
    return s->top == 0;
}

// Returns if stack is full
bool stack_full(Stack *s) {
    return s->top == s->capacity;
}

// Returns stack size
uint32_t stack_size(Stack *s) {
    return s->top;
}

// Pushes item to stack, returns status of push
bool stack_push(Stack *s, Node *n) {
    if (stack_full(s) == false) {
        s->items[s->top] = n;
        s->top = s->top + 1;
        return true;
    }
    return false;
}

// Pops item from stack, returns status of pop
bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) {
        return false;
    }

    s->top = s->top - 1;
    *n = s->items[s->top];
    return true;
}

// Prints the stack
void stack_print(Stack *s) {
    printf("[");
    for (uint32_t i = 0; i < s->top; i = i + 1) {
        node_print(s->items[i]);
        if (i + 1 != s->top) {
            printf(", ");
        }
    }
    printf("]\n");
}
