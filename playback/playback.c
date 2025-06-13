#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "playback.h"

void *playback_thread(void *args) {
    PlaybackArgs *pargs = (PlaybackArgs *)args;
    void *audio_data;

    while (*(pargs->running)) {
        if (ringbuffer_pop(pargs->rb, &audio_data) == 0) {
            // Simulate playback (replace with ALSA or PortAudio later)
            printf("Playing audio data at address: %p\n", audio_data);

            free(audio_data); // Simulate "consuming" the audio data
        } else {
            usleep(1000); // Avoid busy loop
        }
    }

    return NULL;
}
