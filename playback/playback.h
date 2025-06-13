#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "ringbuffer.h"

typedef struct {
    RingBuffer *rb;
    int *running;
} PlaybackArgs;

void *playback_thread(void *args);

#endif
