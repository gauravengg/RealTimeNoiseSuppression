#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "processor.h"

void *processor_thread(void *args) {
    ProcessorArgs *pargs = (ProcessorArgs *)args;
    void *audio_data;

    while (*(pargs->running)) {
        audio_data = ringbuffer_pop(pargs->input_rb);
        if (audio_data != NULL) {
            // TODO: Replace with real noise suppression logic
            void *processed_data = audio_data;  // Just passthrough for now

            ringbuffer_push(pargs->output_rb, processed_data);
        } else {
            usleep(1000); // Sleep to avoid busy-waiting when buffer is empty
        }
    }

    return NULL;
}
