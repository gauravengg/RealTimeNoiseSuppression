#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>


#include "ringbuffer/ringbuffer.h"
#include "capture/capture.h"
#include "processor/processor.h"
#include "playback/playback.h"

#define RINGBUFFER_SIZE 64

// Global running flag and ringbuffers (needed in signal handler)
int running = 1;
RingBuffer input_rb, output_rb;

// Wake up threads on Ctrl+C
void handle_sigint(int sig) {
    (void)sig;
    printf("\nSIGINT received. Shutting down...\n");
    running = 0;

    // Wake up threads blocked on ringbuffers
    ringbuffer_wake_all(&input_rb);
    ringbuffer_wake_all(&output_rb);
}

int main() {
    // Register SIGINT handler
    signal(SIGINT, handle_sigint);

    // Initialize ring buffers
    ringbuffer_init(&input_rb, RINGBUFFER_SIZE);
    ringbuffer_init(&output_rb, RINGBUFFER_SIZE);

    // Set up thread arguments
    CaptureArgs cap_args = { &input_rb, &running };
    ProcessorArgs proc_args = { &input_rb, &output_rb, &running };
    PlaybackArgs play_args = { &output_rb, &running };

    // Launch threads
    pthread_t cap_tid, proc_tid, play_tid;
    pthread_create(&cap_tid, NULL, capture_thread, &cap_args);
    usleep(500000);
    pthread_create(&proc_tid, NULL, processor_thread, &proc_args);
    pthread_create(&play_tid, NULL, playback_thread, &play_args);

    // Wait for threads to finish
    pthread_join(cap_tid, NULL);
    pthread_join(proc_tid, NULL);
    pthread_join(play_tid, NULL);

    // Cleanup
    ringbuffer_destroy(&input_rb);
    ringbuffer_destroy(&output_rb);

    printf("All threads finished. Exiting.\n");
    return 0;
}
