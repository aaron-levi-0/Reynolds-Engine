#include "audio.h"

#include "logging.h"
#include "miniaudio.h"

/*
    A single static engine instance mirrors the rest of the engine's singleton
    subsystems (window, renderer state, ...). One music slot is kept alive for
    looping playback; sound effects use miniaudio's fire-and-forget path.
*/

static ma_engine engine;
static ma_sound  music;
static bool      initialised  = false;
static bool      music_active = false;

bool audio_init()
{
    if (initialised)
        return true;

    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    {
        REYNOLDS_ERROR("@audio: failed to initialise miniaudio engine (error %d).", result);
        return false;
    }

    initialised = true;
    REYNOLDS_INFO("@audio: miniaudio engine initialised.");
    return true;
}

void audio_shutdown()
{
    if (!initialised)
        return;

    audio_stop_music();
    ma_engine_uninit(&engine);
    initialised = false;
}

void audio_play_sound(const char* path)
{
    if (!initialised)
    {
        REYNOLDS_WARN("@audio: audio_play_sound called before audio_init.");
        return;
    }

    ma_result result = ma_engine_play_sound(&engine, path, NULL);
    if (result != MA_SUCCESS)
        REYNOLDS_ERROR("@audio: failed to play '%s' (error %d).", path, result);
    
    REYNOLDS_VERBOSE("@audio: playing sound '%s'.", path);
}

bool audio_play_music(const char* path)
{
    if (!initialised)
    {
        REYNOLDS_WARN("@audio: audio_play_music called before audio_init.");
        return false;
    }

    audio_stop_music();   /* replace any track already playing */

    ma_result result = ma_sound_init_from_file(&engine, path,
                                               MA_SOUND_FLAG_STREAM, NULL, NULL, &music);
    if (result != MA_SUCCESS)
    {
        REYNOLDS_ERROR("@audio: failed to load music '%s' (error %d).", path, result);
        return false;
    }

    ma_sound_set_looping(&music, MA_TRUE);
    ma_sound_start(&music);
    music_active = true;

    REYNOLDS_INFO("@audio: playing music '%s'.", path);
    return true;
}

void audio_stop_music()
{
    if (!initialised || !music_active)
        return;

    ma_sound_uninit(&music);   /* stops playback and frees the sound */
    music_active = false;
}

void audio_set_master_volume(float volume)
{
    if (!initialised)
        return;

    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    ma_engine_set_volume(&engine, volume);

    REYNOLDS_VERBOSE("@audio: master volume set to %.3f.", volume);
}

/* Loops the range [start, end] seconds. end <= 0 means "to the end of the file". */
bool audio_play_music_section(const char* path, float loop_start_sec, float loop_end_sec)
{
    if (!initialised) { REYNOLDS_WARN("@audio: play_music_section before audio_init."); return false; }

    audio_stop_music();

    ma_result result = ma_sound_init_from_file(&engine, path, MA_SOUND_FLAG_STREAM, NULL, NULL, &music);
    if (result != MA_SUCCESS) { REYNOLDS_ERROR("@audio: failed to load '%s' (error %d).", path, result); return false; }

    /* Loop points live in the data source's PCM frames. Convert seconds using
       that same data source's sample rate so the maths is self-consistent. */
    ma_uint32 sample_rate = 0;
    ma_sound_get_data_format(&music, NULL, NULL, &sample_rate, NULL, 0);

    ma_uint64 loop_beg = (ma_uint64)((double)loop_start_sec * sample_rate);
    ma_uint64 loop_end = (loop_end_sec > 0.0f)
        ? (ma_uint64)((double)loop_end_sec * sample_rate)
        : (ma_uint64)-1;                    /* -1 == end of the data */

    /* Clamp to the real length when it's known, so an over-long range can't misbehave. */
    ma_uint64 length = 0;
    if (ma_sound_get_length_in_pcm_frames(&music, &length) == MA_SUCCESS && length > 0)
        if (loop_end == (ma_uint64)-1 || loop_end > length) loop_end = length;

    ma_data_source_set_loop_point_in_pcm_frames(ma_sound_get_data_source(&music), loop_beg, loop_end);
    ma_sound_set_looping(&music, MA_TRUE);
    ma_sound_start(&music);
    music_active = true;

    REYNOLDS_INFO("@audio: playing '%s' from %.3f to %.3f seconds.", path, loop_start_sec, loop_end_sec);
    return true;
}