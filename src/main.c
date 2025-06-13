#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "ringbuffer/ringbuffer.h"
#include "capture/capture.h"
#include "processor/processor.h"
#include "playback/playback.h"

#define RINGBUFFER_SIZE 16

int running = 1;

void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

int main() {
    signal(SIGINT, handle_sigint);

    // Step 1: Initialize ring buffers
    RingBuffer input_rb, output_rb;
    ringbuffer_init(&input_rb, RINGBUFFER_SIZE);
    ringbuffer_init(&output_rb, RINGBUFFER_SIZE);

    // Step 2: Set up arguments
    CaptureArgs cap_args = { &input_rb, &running };
    ProcessorArgs proc_args = { &input_rb, &output_rb, &running };
    PlaybackArgs play_args = { &output_rb, &running };

    // Step 3: Start threads
    pthread_t cap_tid, proc_tid, play_tid;
    pthread_create(&cap_tid, NULL, capture_thread, &cap_args);
    pthread_create(&proc_tid, NULL, processor_thread, &proc_args);
    pthread_create(&play_tid, NULL, playback_thread, &play_args);

    // Step 4: Wait for threads to finish
    pthread_join(cap_tid, NULL);
    pthread_join(proc_tid, NULL);
    pthread_join(play_tid, NULL);

    // Step 5: Cleanup
    ringbuffer_destroy(&input_rb);
    ringbuffer_destroy(&output_rb);

    printf("All threads finished. Exiting.\n");
    return 0;
}
