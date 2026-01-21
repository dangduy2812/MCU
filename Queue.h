/* --- START OF FILE Queue.h --- */
#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>
#include <stdbool.h>

#define QUEUE_CAPACITY 4
#define QUEUE_LINE_MAX 128

typedef struct {
    char items[QUEUE_CAPACITY][QUEUE_LINE_MAX];
    size_t lengths[QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
} LineQueue_t;

void Queue_Init(LineQueue_t* q);
bool Queue_Push(LineQueue_t* q, const char* line, size_t len);
bool Queue_Pop(LineQueue_t* q, char* out, size_t* out_len);
bool Queue_IsEmpty(const LineQueue_t* q);
bool Queue_IsFull(const LineQueue_t* q);

#endif /* QUEUE_H_ */
