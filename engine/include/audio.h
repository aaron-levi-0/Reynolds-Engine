#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>
#include "defines.h"

/*
    Reynolds audio — a thin wrapper over miniaudio's high-level engine.

    Covers sound effects and a single looping music track. Call audio_init() once 
    after start-up and audio_shutdown() before exit.

        audio_init();
        audio_play_music("res/audio/theme.ogg");   // loops until stopped
        ...
        audio_play_sound("res/audio/click.wav");    // overlapping one-shots
        ...
        audio_shutdown();

    Paths are anything miniaudio can decode (wav / flac / mp3, plus others when
    the matching decoders are enabled). Every call is a no-op (and logs) if the
    engine failed to initialise, so callers don't need to guard each one.
*/

REN_API bool audio_init();
REN_API void audio_shutdown();

/* one shot sounds; successive calls overlap. */
REN_API void audio_play_sound(const char* path);

/* Start a looping track, replacing any current one. Returns false on failure. */
REN_API bool audio_play_music(const char* path);
REN_API void audio_stop_music();

/* Master volume, 0.0 (silent) .. 1.0 (full); out-of-range values are clamped. */
REN_API void audio_set_master_volume(float volume);

 /* Loops the range [start, end] seconds. end <= 0 means "to the end of the file". */
REN_API bool audio_play_music_section(const char* path, float start, float end);
  

#endif /* AUDIO_H */