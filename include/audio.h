#ifndef AUDIO_H_
#define AUDIO_H_

#include "core.h"
#include <opus/opusfile.h>

#define NUM_SLOTS 8

typedef struct audio_t {
    AudioDriver driver;
    HwopusDecoder hwdecoder;
    u8* opuspkt_tempbuf;
} audio_t;

typedef AudioDriverWaveBuf audio_track_t;

void audio_init(audio_t *audio);
void audio_exit(audio_t *audio);

void audio_update(audio_t *audio);

void audio_load(audio_track_t *track, audio_t *audio, const char *path);
void audio_play(audio_track_t *track, audio_t *audio, i32 slot);
void audio_set_volume(audio_t *audio, i32 slot, f32 volume);
void audio_free(audio_track_t *track);

#endif
