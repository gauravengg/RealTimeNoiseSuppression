#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "playback.h"

void *playback_thread(void *args) {
    PlaybackArgs *pargs = (PlaybackArgs *)args;
    void *audio_data;

    while (*(pargs->running)) {
        audio_data = ringbuffer_pop(pargs->rb);
        if (audio_data != NULL) {
            // TODO: Add real playback logic
            printf("Playing audio data...\n");  // Placeholder
            free(audio_data); // Clean up if dynamically allocated
        } else {
            usleep(1000); // Avoid busy-waiting
        }
    }

    return NULL;
}
