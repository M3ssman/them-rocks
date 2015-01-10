#include "Framework.h"

int Sound_Explosion_Wrapper(void* data) {
    void Sound_Explosion_Init(const char* file);
    const char* file = (const char*) data;

    Sound_Explosion_Init(file);
    return (7);
}

void Sound_Explosion_Init(const char* file) {

    SDL_AudioSpec* fmt;
    fmt = malloc(sizeof(SDL_AudioSpec));

    fmt->freq = 22050;
    fmt->format = AUDIO_S16;
    fmt->channels = 2;
    fmt->samples = 1024;
    fmt->callback = Sound_Explosion_Callback_Mixer;
    fmt->userdata = NULL;

    if (SDL_OpenAudio(fmt, NULL) < 0) {
        fprintf(stderr, "\nUnable to open audio: %s", SDL_GetError());
        exit(1);
    }
    free(fmt);

    // SDL_Thread* playThread = calloc(1, sizeof(SDL_Thread*));
    // playThread = SDL_CreateThread(Sound_Explosion_Play_Wrapper, file);
    Sound_Explosion_Play(file);
    SDL_Delay(500);
    SDL_CloseAudio();
}

int Sound_Explosion_Play_Wrapper(void* data) {
    const char* file = (const char*) data;
    Sound_Explosion_Play(file);
    SDL_Delay(500);
    return (7);
}

void Sound_Explosion_Play(const char *file) {
    int index;
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;
    SDL_AudioCVT cvt;

    for (index = 0; index < NUM_SOUNDS; ++index) {
        if (sounds[index].dpos == sounds[index].dlen) {
            break;
        }
    }
    if (index == NUM_SOUNDS)
        return;

    // Load the sound file and convert it to 16-bit stereo at 22kHz
    if (SDL_LoadWAV(file, &wave, &data, &dlen) == NULL) {
        fprintf(stderr, "Couldn't load %s: %sn", file, SDL_GetError());
        return;
    } else {
        printf("\nfile %s loaded", file);
    }

    SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, AUDIO_S16, 2, 22050);
    cvt.buf = malloc(dlen * cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;

    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(data);

    if (sounds[index].data) {
        free(sounds[index].data);
    }
    SDL_LockAudio();
    sounds[index].data = cvt.buf;
    sounds[index].dlen = cvt.len_cvt;
    sounds[index].dpos = 0;
    SDL_UnlockAudio();
    SDL_PauseAudio(0);
}

void Sound_Explosion_Callback_Mixer(void *unused, Uint8 *stream, int len) {
    int i;
    Uint32 amount;
    for (i = 0; i < NUM_SOUNDS; ++i) {
        amount = (sounds[i].dlen - sounds[i].dpos);
        if (amount > len) {
            amount = len;
        }
        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, SDL_MIX_MAXVOLUME);
        sounds[i].dpos += amount;
    }
}
