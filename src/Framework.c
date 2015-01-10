#include "Framework.h"
#include <stdio.h>

// give a value
// how width is the game window
const uint16_t GAME_WINDOW_WIDTH = 800;
const uint16_t GAME_WINDOW_HEIGHT = 600;

int Framework_Init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) == -1) {
        printf("\nCant init SDL Subsystems : %s", SDL_GetError());
        SDL_Quit();
        return (0);
    }

    SDL_WM_SetCaption("openAsteroids with ANSI-C and SDL Multimedia Library", "AsteroidIcon");
    SDL_Surface* icon = SDL_LoadBMP("Data/AsteroidIcon.bmp");
    SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 255, 0, 255));
    SDL_WM_SetIcon(icon, NULL);	// make app crash when called AFTER SDL_SetVideoMode
    SDL_ShowCursor(0);

    FRAMEWORK.pScreen = SDL_SetVideoMode(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
    FRAMEWORK.pKeystate = SDL_GetKeyState(NULL);

    if (FRAMEWORK.pScreen == 0 || FRAMEWORK.pKeystate == 0) {
        fprintf(stderr, "FRAMEWORK-Init-Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    } else {
        printf("\nFramework initialised");
    }
    Timer_Start();
    printf("\nGameTimer started at %f\n", TIMER.fCurTime);
    return 1;
}

void Framework_Quit() {
    FRAMEWORK.pScreen = 0;
    FRAMEWORK.pKeystate = 0;
    SDL_Quit();
}

void Framework_Update() {
    Timer_Update();
    SDL_PumpEvents();
}
void Framework_Clear() {
    SDL_FillRect(FRAMEWORK.pScreen, NULL, SDL_MapRGB(FRAMEWORK.pScreen->format, 0, 0, 0));
}

int Framework_KeyDown(int SDL_Key_ID) {
    assert(SDL_Key_ID != 0);
    return (FRAMEWORK.pKeystate[SDL_Key_ID] ? 1 : 0);
}

void Framework_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
////    SDL_BlitSurface(pSprite->pImage, &pSprite->FrameRectangle, FRAMEWORK.pScreen, &pSprite->Rectangle);
    SDL_BlitSurface(src, srcrect, dst, dstrect);
}
SDL_Surface* Framework_LoadBMPFile(const char* file) {
    return SDL_LoadBMP(file);
}
void Framework_ShutDown() {
    SDL_Quit();
}

void Timer_Start() {
    TIMER.fCurTime = 0.0f;
    TIMER.fLastTime = SDL_GetTicks() / 1000.0f;
    TIMER.fElapsed = 0.0f;
}

void Timer_Update() {
    TIMER.fCurTime = SDL_GetTicks() / 1000.0f;
    TIMER.fElapsed = TIMER.fCurTime - TIMER.fLastTime;
    TIMER.fLastTime = TIMER.fCurTime;
}
