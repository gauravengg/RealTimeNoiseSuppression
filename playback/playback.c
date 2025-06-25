#include "playback.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <string.h>

#define SAMPLE_RATE 16000
#define CAPTURE_CHUNK 1600
#define PLAYBACK_CHUNK 1600// collect 10 chunks

void *playback_thread(void *args) {
    PlaybackArgs *pargs = (PlaybackArgs *)args;
    RingBuffer *rb = pargs->input_rb;
    int *running = pargs->running;

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    int dir;

    snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 1);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
    snd_pcm_hw_params(handle, params);
    snd_pcm_prepare(handle);

    short *playback_buffer = malloc(sizeof(short) * PLAYBACK_CHUNK);

    while (*running) {
        int collected = 0;

        while (collected < PLAYBACK_CHUNK && *running) {
            short *chunk = (short *)ringbuffer_pop(rb);
            if (chunk) {
                memcpy(playback_buffer + collected, chunk, sizeof(short) * CAPTURE_CHUNK);
                collected += CAPTURE_CHUNK;
                free(chunk);
            } else {
                usleep(100);  // wait briefly if buffer is empty
            }
        }

        if (collected == PLAYBACK_CHUNK) {
            int err = snd_pcm_writei(handle, playback_buffer, PLAYBACK_CHUNK);
            if (err < 0) {
                fprintf(stderr, "Playback write error: %s\n", snd_strerror(err));
                snd_pcm_prepare(handle);  // recover from underrun
            } else {
                printf("Playback: Played %d frames\n", err);
            }
        }
    }

    free(playback_buffer);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    printf("Playback thread exiting.\n");
    return NULL;
}
