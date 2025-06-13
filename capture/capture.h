#ifndef CAPTURE_H
#define CAPTURE_H

#include "../ringbuffer/ringbuffer.h"

typedef struct {
    RingBuffer *rb;
    int *running;
} CaptureArgs;

void *capture_thread(void *args);

#endif
