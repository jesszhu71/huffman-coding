// Huffman ADT

#include "code.h"
#include "defines.h"
#include "header.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *pq = pq_create(ALPHABET);
    // create node for everything in histogram that isn't 0 freq.
    for (uint32_t i = 0; i < ALPHABET; i = i + 1) {
        int64_t freq = hist[i];
        if (freq > 0) {
            Node *new_node = node_create(i, freq);
            enqueue(pq, new_node);
        }
    }

    while (pq_size(pq) >= 2) {
        Node *dq_l;
        Node *dq_r;
        // Dequeue left and right
        dequeue(pq, &dq_l);
        dequeue(pq, &dq_r);
        // Join left and right and enqueue
        Node *parent = node_join(dq_l, dq_r);
        enqueue(pq, parent);
    }
    // Get the root of tree
    Node *ret;
    dequeue(pq, &ret);

    // Destroy pq??
    pq_delete(&pq);
    return ret;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    // perform post-order traversal
    static Code c = { 0, { 0 } };
    if (root && root->left == NULL && root->right == NULL) {
        // copy node into table
        table[root->symbol] = c;
        return;
    } else if (root) {
        // Recursively call since not leaf
        code_push_bit(&c, 0);
        //Left node
        build_codes(root->left, table);
        uint8_t temp = 0;
        code_pop_bit(&c, &temp);

        // Right node
        code_push_bit(&c, 1);
        build_codes(root->right, table);
        code_pop_bit(&c, &temp);
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    // Generate stack to hold nodes
    Stack *s = stack_create(ALPHABET);
    for (int i = 0; i < nbytes; i = i + 2) {
        if (tree[i] == 'L') {
            // Generate new node and push onto stack
            Node *new = node_create(tree[i + 1], 0);
            stack_push(s, new);
        } else if (tree[i] == 'I') {
            Node *pop_r;
            Node *pop_l;
            // Pop right and left
            stack_pop(s, &pop_r);
            stack_pop(s, &pop_l);
            // Join under parent and push back to stack
            Node *parent = node_join(pop_l, pop_r);
            stack_push(s, parent);

            i = i - 1; //When reading i, there is no symbol after it.
        }
    }

    // Pop the root node
    Node *root;
    stack_pop(s, &root);

    // Delete stack
    stack_delete(&s);
    return root;
}

void delete_tree(Node **root) {
    if ((*root) && (*root)->left == NULL && (*root)->right == NULL) {
        // Is leaf, delete the leaf
        node_delete(root);
        return;
    } else if (*root) {
        // traverse left and right
        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);
        node_delete(root);
    }
}
