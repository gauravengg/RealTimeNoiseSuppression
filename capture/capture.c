#include "capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *capture_thread(void *args) {
    CaptureArgs *cap_args = (CaptureArgs *)args;
    RingBuffer *rb = cap_args->rb;
    int *running = cap_args->running;

    while (*running) {
        int *sample = malloc(sizeof(int));
        *sample = rand() % 100;  // fake audio sample
        ringbuffer_push(rb, sample);
        printf("[Capture] Sample captured: %d\n", *sample);
        usleep(50000); // simulate 20Hz mic sampling
    }

    return NULL;
}
