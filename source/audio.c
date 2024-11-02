#include "audio.h"
#include "util.h"
#include <malloc.h>
#include <string.h>

const static size_t opuspkt_tmpbuf_size = sizeof(HwopusHeader) + 4096*48;

static int hw_decode_callback(void *_ctx, OpusMSDecoder *_decoder, void *_pcm, const ogg_packet *_op, int _nsamples, int _nchannels, int _format, int _li) {
    audio_t *audio = (audio_t *)_ctx;

    u8 *opuspkt_tmpbuf = audio->opuspkt_tempbuf;
    HwopusHeader *hdr = NULL;
    u64 pktsize, pktsize_extra;

    Result rc = 0;
    s32 DecodedDataSize = 0;
    s32 DecodedSampleCount = 0;

    if (_format != OP_DEC_FORMAT_SHORT) return OPUS_BAD_ARG;

    pktsize = _op->bytes;
    pktsize_extra = pktsize+8;

    if (pktsize_extra > opuspkt_tmpbuf_size) return OPUS_INTERNAL_ERROR;

    hdr = (HwopusHeader*)opuspkt_tmpbuf;
    memset(opuspkt_tmpbuf, 0, pktsize_extra);

    hdr->size = __builtin_bswap32(pktsize);
    memcpy(&opuspkt_tmpbuf[sizeof(HwopusHeader)], _op->packet, pktsize);

    rc = hwopusDecodeInterleaved(&audio->hwdecoder, &DecodedDataSize, &DecodedSampleCount, opuspkt_tmpbuf, pktsize_extra, _pcm, _nsamples * _nchannels * sizeof(opus_int16));

    if (R_FAILED(rc)) return OPUS_INTERNAL_ERROR;
    if (DecodedDataSize != pktsize_extra || DecodedSampleCount != _nsamples) return OPUS_INVALID_PACKET;

    return 0;
}

void audio_init(audio_t *audio) {
    memset(audio, 0, sizeof(audio_t));

    Result res;
    res = hwopusDecoderInitialize(&audio->hwdecoder, 48000, 1);
    if (R_FAILED(res)) {
        throw_error("Audio error", "Failed to initialize hw decoder", res);
    }

    static const AudioRendererConfig audio_config =
    {
        .output_rate     = AudioRendererOutputRate_48kHz,
        .num_voices      = 24,
        .num_effects     = 0,
        .num_sinks       = 1,
        .num_mix_objs    = 1,
        .num_mix_buffers = 2,
    };

    res = audrenInitialize(&audio_config);
    if (R_FAILED(res)) {
        throw_error("Audio error", "Failed to initialize audio renderer", res);
    }

    res = audrvCreate(&audio->driver, &audio_config, 2);
    if (R_FAILED(res)) {
        throw_error("Audio error", "Failed to initialize audio driver", res);
    }

    static const u8 sink_channels[] = { 0, 1 };
    audrvDeviceSinkAdd(&audio->driver, AUDREN_DEFAULT_DEVICE_NAME, 2, sink_channels);

    audrvUpdate(&audio->driver);
    audrenStartAudioRenderer();

    audio->opuspkt_tempbuf = malloc(opuspkt_tmpbuf_size);

    for (i32 i = 0; i < NUM_SLOTS; i++) {
        audrvVoiceInit(&audio->driver, i, 1, PcmFormat_Int16, 48000);
        audrvVoiceSetDestinationMix(&audio->driver, i, AUDREN_FINAL_MIX_ID);
        audrvVoiceSetMixFactor(&audio->driver, i, 1.0f, 0, 0);
        audrvVoiceSetMixFactor(&audio->driver, i, 1.0f, 0, 1);
        audrvVoiceStart(&audio->driver, i);

        audio->fade_distances[i] = INFINITY;
        audio->volumes[i] = 1.0f;
    }
}

void audio_exit(audio_t *audio) {
    hwopusDecoderExit(&audio->hwdecoder);
    audrvClose(&audio->driver);
    audrenExit();
    free(audio->opuspkt_tempbuf);
}

void audio_update(audio_t *audio, HMM_Vec3 *camera_position, HMM_Vec3 *camera_front, HMM_Vec3 *camera_right) {
    for (u32 i = 0; i < NUM_SLOTS; i++) {
        if (audio->position_enabled & (1 << i)) {
            HMM_Vec3 cam_to_audio = HMM_SubV3(audio->positions[i], *camera_position);
            f32 distance = HMM_LenV3(cam_to_audio);
            
            // @note(kripesh): Spatialization (in XZ plane only)
            cam_to_audio.Y = 0.0f;
            HMM_Vec3 cam_to_audio_xz = HMM_NormV3(cam_to_audio);
            HMM_Vec3 camera_front_xz = HMM_NormV3(HMM_V3(camera_front->X, 0.0f, camera_front->Z));

            f32 center = HMM_DotV3(cam_to_audio_xz, camera_front_xz);
            f32 right = HMM_DotV3(cam_to_audio_xz, *camera_right);

            if (center < 0.0f) {
                center = HMM_MAX(0.0f, HMM_ABS(center) - 0.15f);
            }

            f32 final_right = HMM_MIN(center + HMM_MAX(0.0f, right), 1.0f);
            f32 final_left = HMM_MIN(center + HMM_MAX(0.0f, -right), 1.0f);

            // @note(kripesh): Attenuation
            f32 factor = 1.0f;
            if (distance > audio->fade_distances[i]) {
                if (distance > audio->cutoff_distances[i]) {
                    factor = 0.0f;
                } else {
                    factor = 1.0f - (distance - audio->fade_distances[i]) / (audio->cutoff_distances[i] - audio->fade_distances[i]);
                }
            }

            factor *= audio->volumes[i];
            final_right *= factor;
            final_left *= factor;

            audrvVoiceSetMixFactor(&audio->driver, i, final_right, 0, 0);
            audrvVoiceSetMixFactor(&audio->driver, i, final_left, 0, 1);
        }
    }
    
    audrvUpdate(&audio->driver);
}

void audio_load(audio_track_t *track, audio_t *audio, const char *path) {
    memset(track, 0, sizeof(audio_track_t));

    OggOpusFile *audio_file = op_open_file(path, NULL);
    op_set_decode_callback(audio_file, hw_decode_callback, audio);
    i64 num_samples = op_pcm_total(audio_file, -1);

    if (op_channel_count(audio_file, -1) != 1) {
        throw_error("Audio error", "Loaded a file with 2 or more channels. Only mono is supported currently!", -1);
    }

    // @note(kripesh): mempools must be 4096 byte aligned
    u64 mempool_size = (num_samples * sizeof(i16) + 0xFFF) &~ 0xFFF;
    track->data_pcm16 = memalign(0x1000, mempool_size);
    track->size = num_samples * sizeof(i16);
    track->start_sample_offset = 0;
    track->end_sample_offset = num_samples;

    // @note(kripesh): load entire file into the mempool
    i32 total_read = 0;

    while (total_read < num_samples) {
        i32 samples_read = op_read(audio_file, track->data_pcm16 + total_read, mempool_size - total_read * sizeof(i16), NULL);
        if (samples_read < 0) {
            throw_error("Audio error", "Failed to load audio file", samples_read);
        }
        
        total_read += samples_read;
        
        if (samples_read == 0) {
            break;
        }
    }
    op_free(audio_file);

    armDCacheFlush(track->data_pcm16, mempool_size);
    i32 mpid = audrvMemPoolAdd(&audio->driver, track->data_pcm16, mempool_size);
    audrvMemPoolAttach(&audio->driver, mpid);
}

void audio_free(audio_track_t *track) {
    free(track->data_pcm16);
}

void audio_play(audio_track_t *track, audio_t *audio, i32 slot) {
    audrvVoiceStop(&audio->driver, slot);
    audrvVoiceAddWaveBuf(&audio->driver, slot, track);
    audrvVoiceStart(&audio->driver, slot);
}

void audio_set_volume(audio_t *audio, i32 slot, f32 volume) {
    audio->volumes[slot] = volume;

    if (!(audio->position_enabled & (1 << slot))) {
        audrvVoiceSetMixFactor(&audio->driver, slot, volume, 0, 0);
        audrvVoiceSetMixFactor(&audio->driver, slot, volume, 0, 1);
    }
}

void audio_set_position(audio_t *audio, i32 slot, HMM_Vec3 position) {
    audio->position_enabled |= (1 << slot);
    audio->positions[slot] = position;
}

void audio_set_attenuation(audio_t *audio, i32 slot, f32 fade_distance, f32 cutoff_distance) {
   audio->fade_distances[slot] = fade_distance;
   audio->cutoff_distances[slot] = cutoff_distance;
}

void audio_set_loop(audio_track_t *track, b8 loop) {
    track->is_looping = loop;
}
