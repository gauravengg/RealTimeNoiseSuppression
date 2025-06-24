#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "../ringbuffer/ringbuffer.h"

typedef struct {
    RingBuffer *input_rb;
    int *running;
} PlaybackArgs;

void *playback_thread(void *args);

#endif
