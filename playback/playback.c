#include "playback.h"
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SAMPLE_RATE 16000
#define BUFFER_FRAMES 1

void *playback_thread(void *args) {
    PlaybackArgs *pargs = (PlaybackArgs *)args;
    RingBuffer *rb = pargs->input_rb;
    int *running = pargs->running;

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    int dir;

    // Open playback device
    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        perror("Playback open failed");
        return NULL;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 1);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size(handle, params, BUFFER_FRAMES, dir);
    snd_pcm_hw_params(handle, params);
    snd_pcm_prepare(handle);

    short buffer[BUFFER_FRAMES];

    while (*running) {
        short *sample = (short *)ringbuffer_pop(rb);
        if (sample != NULL) {
            buffer[0] = *sample;
            int err = snd_pcm_writei(handle, buffer, BUFFER_FRAMES);
            if (err < 0) {
                fprintf(stderr, "Playback write error: %s\n", snd_strerror(err));
                snd_pcm_prepare(handle);  // recover from error
            }
            free(sample);
        } else {
            usleep(1000);
        }
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    printf("Playback thread exiting.\n");
    return NULL;
}
