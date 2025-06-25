// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include "processor.h"
// #include "../whisper/whisper_client.h" 
// #define CHUNK_SIZE 1600  

// void *processor_thread(void *args) {
//     ProcessorArgs *pargs = (ProcessorArgs *)args;
//     void *audio_data;

//     while (*(pargs->running)) {
//         audio_data = ringbuffer_pop(pargs->input_rb);
//         if (audio_data != NULL) {
//             // TODO: Add real noise suppression
//             void *processed_data = audio_data; 
//             //send_audio_to_whisper((short *)processed_data, CHUNK_SIZE);  // Just passthrough for now

//             ringbuffer_push(pargs->output_rb, processed_data);
//         } else {
//             usleep(1000); // Avoid busy waiting if nothing to process
//         }
//     }

//     printf("Processor thread exiting.\n");
//     return NULL;
// }
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "processor.h"
#include "../whisper/whisper_client.h"  // Include Whisper client

#define CHUNK_SIZE 1600  // Same as CAPTURE_CHUNK

// This runs in a separate thread to avoid blocking the processor thread
void *send_to_whisper_async(void *arg) {
    short *audio_copy = (short *)arg;

    // Send audio to Whisper server
    send_audio_to_whisper(audio_copy, CHUNK_SIZE);

    // Free allocated memory
    free(audio_copy);
    return NULL;
}

void *processor_thread(void *args) {
    ProcessorArgs *pargs = (ProcessorArgs *)args;
    void *audio_data;

    while (*(pargs->running)) {
        audio_data = ringbuffer_pop(pargs->input_rb);
        if (audio_data != NULL) {
            // Send to Whisper in a background thread (non-blocking)
            short *copy = malloc(sizeof(short) * CHUNK_SIZE);
            if (copy != NULL) {
                memcpy(copy, audio_data, sizeof(short) * CHUNK_SIZE);
                pthread_t whisper_thread;
                pthread_create(&whisper_thread, NULL, send_to_whisper_async, (void *)copy);
                pthread_detach(whisper_thread);
            }

            // Forward to playback
            ringbuffer_push(pargs->output_rb, audio_data);
        } else {
            usleep(1000); // Avoid busy waiting
        }
    }

    printf("Processor thread exiting.\n");
    return NULL;
}


