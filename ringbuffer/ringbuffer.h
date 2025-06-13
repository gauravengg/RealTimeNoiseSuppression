#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <pthread.h>

typedef struct {
    void **data;
    int size;
    int start;
    int end;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} RingBuffer;

void ringbuffer_init(RingBuffer *rb, int size);
void ringbuffer_destroy(RingBuffer *rb);
void ringbuffer_push(RingBuffer *rb, void *item);
void *ringbuffer_pop(RingBuffer *rb);

#endif

