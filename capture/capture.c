#include "capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <string.h>

#define SAMPLE_RATE 16000
#define CAPTURE_CHUNK 1600  // 0.1 seconds

void *capture_thread(void *args) {
    CaptureArgs *cap_args = (CaptureArgs *)args;
    RingBuffer *rb = cap_args->rb;
    int *running = cap_args->running;

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    int dir;

    snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 1);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
    snd_pcm_hw_params(handle, params);
    snd_pcm_prepare(handle);

    short *buffer = malloc(sizeof(short) * CAPTURE_CHUNK);

    while (*running) {
        int err = snd_pcm_readi(handle, buffer, CAPTURE_CHUNK);
        if (err == -EPIPE) {
            snd_pcm_prepare(handle);
            continue;
        }

        if (err > 0) {
            short *chunk = malloc(sizeof(short) * CAPTURE_CHUNK);
            memcpy(chunk, buffer, sizeof(short) * CAPTURE_CHUNK);
            ringbuffer_push(rb, chunk);
            printf("[Capture] Pushed %d frames\n", err);
        }

        usleep(500);  // reduce CPU usage
    }

    snd_pcm_drop(handle);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);

    printf("Capture thread exiting.\n");
    return NULL;
}
