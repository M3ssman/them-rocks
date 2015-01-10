#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include <SDL.h>
#include <assert.h>

// how width is the game window
const uint16_t GAME_WINDOW_WIDTH;
const uint16_t GAME_WINDOW_HEIGHT;

typedef struct {
    SDL_Surface *pPicture;
} Framework_Pic;

typedef struct {
    SDL_Surface* pScreen;
    Uint8* pKeystate;
} Framework;

typedef struct {
    float fElapsed;  // Vergangene Zeit seit dem letzten Frame
    float fCurTime;  // Aktuelle Zeit
    float fLastTime; // Zeit des letzten Frames
} Timer;

int Framework_Init();
void Framework_Quit();
int Framework_KeyDown(int SDL_Key_ID);
int Framework_MouseAction(int SDL_Mouse_Action);
void Framework_Update();
void Framework_Clear();
void Timer_Start();
void Timer_Update();

// Sound Definitions
#define NUM_SOUNDS 4
struct sample {
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
} sounds[NUM_SOUNDS];
SDL_AudioSpec fmt;

int Sound_Explosion_Wrapper(void*);
void Sound_Explosion_Callback_Mixer(void *unused, Uint8 *stream, int len);
int Sound_Explosion_Play_Wrapper(void* data);
void Sound_Explosion_Play(const char *file);

Framework FRAMEWORK;
Timer TIMER;

// try to collect SDL calls
void Framework_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
SDL_Surface* Framework_LoadBMPFile(const char* file);
void Framework_ShutDown();

// try to collect SDL KeyEvents
//const int Framwork_Key_Left = SDLKey.SDLK_LEFT;

#endif
