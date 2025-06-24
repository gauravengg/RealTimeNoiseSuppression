#include "capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#define SAMPLE_RATE 16000
#define BUFFER_FRAMES 256

void *capture_thread(void *args) {
    CaptureArgs *cap_args = (CaptureArgs *)args;
    RingBuffer *rb = cap_args->rb;
    int *running = cap_args->running;

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    int dir;

    // Open PCM device
    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
        fprintf(stderr, "Error opening PCM device\n");
        return NULL;
    }
    printf("[DEBUG] PCM device opened\n");

    // Allocate hardware params
    snd_pcm_hw_params_malloc(&params);
    printf("[DEBUG] Allocated hw params\n");

    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 1);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size(handle, params, BUFFER_FRAMES, dir);
    snd_pcm_hw_params(handle, params);
    snd_pcm_prepare(handle);
    printf("[DEBUG] PCM parameters set. Rate = %u\n", rate);

    short buffer[BUFFER_FRAMES];

    while (*running) {
        int err = snd_pcm_readi(handle, buffer, BUFFER_FRAMES);
        if (err == -EPIPE) {
            fprintf(stderr, "[DEBUG] Overrun occurred, recovering...\n");
            snd_pcm_prepare(handle);
            continue;
        } else if (err < 0) {
            fprintf(stderr, "[ERROR] Read error: %s\n", snd_strerror(err));
            continue;
        }

        if (err > 0) {
            for(int i =0;i<err;i++){
                short *sample = malloc(sizeof(short));
                *sample = buffer[i];
                ringbuffer_push(rb, sample);
                printf("[Capture] Mic sample: %d\n", *sample);
            }
        }

        usleep(500); // slow down the loop a bit
    }

    snd_pcm_drop(handle);  // break blocking if stuck
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    printf("Capture thread exiting.\n");
    return NULL;
}
