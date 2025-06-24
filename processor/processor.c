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
            // TODO: Add real noise suppression
            void *processed_data = audio_data;  // Just passthrough for now

            ringbuffer_push(pargs->output_rb, processed_data);
        } else {
            usleep(1000); // Avoid busy waiting if nothing to process
        }
    }

    printf("Processor thread exiting.\n");
    return NULL;
}
