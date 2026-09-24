// Priority Queue

#include "pq.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// PQ struct
struct PriorityQueue {
    uint32_t capacity;
    Node **items;
    int high; // high frequency, low priority
    int low; // low frequency, high priority
    uint32_t size;
};

// Create PQ
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->high = 0;
        q->low = 0;
        q->size = 0;
        q->capacity = capacity;
        q->items = (Node **) calloc(capacity, sizeof(Node *));
        if (!q->items) {
            free(q);
            q = NULL;
        }
    }
    return q;
}

// Deletes PQ
void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->items) {
        free((*q)->items);
        free(*q);
        *q = NULL;
    }
}

// Returns if pq is empty
bool pq_empty(PriorityQueue *q) {
    if (q->size == 0) {
        return true;
    }
    return false;
}

// Returns if pq is full
bool pq_full(PriorityQueue *q) {
    if (q->size == q->capacity) {
        return true;
    }
    return false;
}

// Returns size of pq
uint32_t pq_size(PriorityQueue *q) {
    return (uint32_t) q->size;
}

// Enqueues a node, from the q->high size
// low freq->high freq, dequeue from low
bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_empty(q)) {
        // Queue is empty, just node at index 0
        q->items[q->low] = n;
        q->size = q->size + 1;
        q->high = (q->high + 1) % q->capacity;
        return true;
    } else if (pq_full(q)) {
        // Queue is full, can't add
        return false;
    }
    for (int i = q->high; i != q->low; i = (i - 1) % q->capacity) {
        // Is current empty space for n or moving over in array
        if ((q->items[i - 1])->frequency > n->frequency) {
            //move item in array over
            q->items[i] = q->items[(i - 1) % q->capacity];
            if ((i - 1) % (int) q->capacity == q->low) {
                q->items[q->low] = n;
                break;
            }
        } else {
            q->items[i] = n;
            break;
        }
    }
    q->size = q->size + 1;
    q->high = (q->high + 1) % q->capacity;
    return true;
}

// Dequeues a node, from the q->low size (low freq = high priority);
bool dequeue(PriorityQueue *q, Node **n) {
    if (!q || pq_empty(q)) {
        return false;
    }
    *n = q->items[q->low];
    q->low = (q->low + 1) % q->capacity;
    q->size = q->size - 1;
    return true;
}

// Prints the pq
void pq_print(PriorityQueue *q) {
    printf("low freq -> high freq\n");
    for (uint32_t i = 0; i < q->size; i = i + 1) {
        node_print(q->items[(q->low + i) % q->capacity]);
    }
}
