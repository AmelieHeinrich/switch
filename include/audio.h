#ifndef AUDIO_H_
#define AUDIO_H_

#include "core.h"
#include <opus/opusfile.h>

#define NUM_SLOTS 8

typedef struct audio_t {
    AudioDriver driver;
    HwopusDecoder hwdecoder;
    u8* opuspkt_tempbuf;

    // @note(kripesh): slot specifics
    u32 position_enabled;
    HMM_Vec3 positions[NUM_SLOTS];
    f32 volumes[NUM_SLOTS];
    f32 fade_distances[NUM_SLOTS];
    f32 cutoff_distances[NUM_SLOTS];
} audio_t;

typedef AudioDriverWaveBuf audio_track_t;

void audio_init(audio_t *audio);
void audio_exit(audio_t *audio);

void audio_update(audio_t *audio, HMM_Vec3 *camera_position, HMM_Vec3 *camera_front, HMM_Vec3 *camera_right);

void audio_load(audio_track_t *track, audio_t *audio, const char *path);
void audio_play(audio_track_t *track, audio_t *audio, i32 slot);
void audio_set_loop(audio_track_t *track, b8 loop);
void audio_set_volume(audio_t *audio, i32 slot, f32 volume);
void audio_set_position(audio_t *audio, i32 slot, HMM_Vec3 position);
void audio_set_attenuation(audio_t *audio, i32 slot, f32 fade_distance, f32 cutoff_distance);
void audio_free(audio_track_t *track);

#endif
