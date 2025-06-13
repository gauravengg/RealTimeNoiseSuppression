#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "ringbuffer.h"

typedef struct {
    RingBuffer *input_rb;
    RingBuffer *output_rb;
    int *running;
} ProcessorArgs;

void *processor_thread(void *args);

#endif

