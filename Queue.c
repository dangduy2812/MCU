/* --- START OF FILE Queue.c --- */
#include "Queue.h"
#include <string.h>

void Queue_Init(LineQueue_t* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

bool Queue_IsEmpty(const LineQueue_t* q) {
    return q->count == 0;
}

bool Queue_IsFull(const LineQueue_t* q) {
    return q->count >= QUEUE_CAPACITY;
}

bool Queue_Push(LineQueue_t* q, const char* line, size_t len) {
    if (Queue_IsFull(q)) return false;
    if (len >= QUEUE_LINE_MAX) return false;

    memcpy(q->items[q->tail], line, len);
    q->items[q->tail][len] = '\0';
    q->lengths[q->tail] = len;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->count++;
    return true;
}

bool Queue_Pop(LineQueue_t* q, char* out, size_t* out_len) {
    if (Queue_IsEmpty(q)) return false;

    size_t len = q->lengths[q->head];
    memcpy(out, q->items[q->head], len + 1);
    if (out_len) *out_len = len;
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->count--;
    return true;
}
