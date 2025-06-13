#include "ringbuffer.h"
#include <stdlib.h>

void ringbuffer_init(RingBuffer *rb, int size) {
    rb->data = malloc(sizeof(void *) * size);
    rb->size = size;
    rb->start = rb->end = rb->count = 0;
    pthread_mutex_init(&rb->mutex, NULL);
    pthread_cond_init(&rb->not_empty, NULL);
    pthread_cond_init(&rb->not_full, NULL);
}

void ringbuffer_destroy(RingBuffer *rb) {
    free(rb->data);
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->not_empty);
    pthread_cond_destroy(&rb->not_full);
}

void ringbuffer_push(RingBuffer *rb, void *item) {
    pthread_mutex_lock(&rb->mutex);
    while (rb->count == rb->size)
        pthread_cond_wait(&rb->not_full, &rb->mutex);
    rb->data[rb->end] = item;
    rb->end = (rb->end + 1) % rb->size;
    rb->count++;
    pthread_cond_signal(&rb->not_empty);
    pthread_mutex_unlock(&rb->mutex);
}

void *ringbuffer_pop(RingBuffer *rb) {
    pthread_mutex_lock(&rb->mutex);
    while (rb->count == 0)
        pthread_cond_wait(&rb->not_empty, &rb->mutex);
    void *item = rb->data[rb->start];
    rb->start = (rb->start + 1) % rb->size;
    rb->count--;
    pthread_cond_signal(&rb->not_full);
    pthread_mutex_unlock(&rb->mutex);
    return item;
}
