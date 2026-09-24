// Node ADT

#include "node.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

// Creates nodes with a symbol, frequency, left and right children
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->left = NULL;
        n->right = NULL;
        n->symbol = symbol;
        n->frequency = frequency;
    }
    return n;
}

// Deletes the current node
void node_delete(Node **n) {
    if (*n) {
        free(*n);
        *n = NULL;
    }
    return;
}

// Joins a left and right node with a parent
Node *node_join(Node *left, Node *right) {
    Node *parent = node_create('$', left->frequency + right->frequency);
    parent->left = left;
    parent->right = right;
    return parent;
}

// Prints the numerical value of the node's symbol
void node_print(Node *n) {
    printf("number: %" PRIu8 "\n", n->symbol);
}
