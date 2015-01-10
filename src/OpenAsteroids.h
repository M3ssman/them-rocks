/*
 * OpenAsteroids.h
 *
 *  Created on: 06.01.2015
 *      Author: u_hartwig
 */
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#ifndef SRC_OPENASTEROIDS_H_
#define SRC_OPENASTEROIDS_H_

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
Framework FRAMEWORK;

typedef struct {
    float fElapsed;  // Vergangene Zeit seit dem letzten Frame
    float fCurTime;  // Aktuelle Zeit
    float fLastTime; // Zeit des letzten Frames
} Timer;
Timer TIMER;

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
    SDL_WM_SetIcon(icon, NULL); // make app crash when called AFTER SDL_SetVideoMode
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

// Sound Definitions
#define NUM_SOUNDS 4
struct sample {
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
} sounds[NUM_SOUNDS];
SDL_AudioSpec fmt;
int Sound_Explosion_Wrapper(void* data) {
    void Sound_Explosion_Init(const char* file);
    const char* file = (const char*) data;
    Sound_Explosion_Init(file);
    return (7);
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
int Sound_Explosion_Play_Wrapper(void* data) {
    const char* file = (const char*) data;
    Sound_Explosion_Play(file);
    SDL_Delay(500);
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

// some central interfaces
// Sprite Definitions
//typedef struct {
//    int16_t x, y;
//    Uint16 w, h;
//} Rectangle;
typedef struct {
    SDL_Surface *pImage;        // Das eigentliche Bild des Sprites
    SDL_Rect Rectangle,            // Rechteckiger Bereich des Sprites
            Clip,                   // Gr��e des Sprites
            FrameRectangle;         // Ausschnitt des Sprites f�r Animationsphase
    uint16_t FrameWidth,             // Breite einer Animationsphase
            FrameHeight;             // H�he einer Animationsphase
    uint8_t NumFramesX;             // Wie viele Anim-Phasen in X-Richtung?
} Sprite;

// forward former SDL calls
/**
 * Copy a Sprites specific frame to a Rectangle of the Game Frame
 */
void OpenAsteroids_BlitSprite(Sprite* pSprite) {
    SDL_BlitSurface(pSprite->pImage, &pSprite->FrameRectangle, FRAMEWORK.pScreen, &pSprite->Rectangle);
}

/**
 * Render complete sprite image
 */
void OpenAsteroids_RenderWholeSprite(Sprite* pSprite) {
    SDL_BlitSurface(pSprite->pImage, NULL, FRAMEWORK.pScreen, &pSprite->Rectangle);
}

/**
 * fill Game Screen totaloy with black at start of game loop
 */
void OpenAsteroids_fillScreenWithBlack() {
    SDL_FillRect(FRAMEWORK.pScreen, NULL, SDL_MapRGB(FRAMEWORK.pScreen->format, 0, 0, 0));
}

/**
 * flip screen buffers
 */
void OpenAsteroids_FlipSCreenBuffer() {
    SDL_Flip(FRAMEWORK.pScreen);
}
void OpenAsteroids_SpriteSetColorKey(Sprite* pSprite, int R, int G, int B) {
    SDL_SetColorKey(pSprite->pImage, SDL_SRCCOLORKEY, SDL_MapRGB(pSprite->pImage->format, R, G, B));
}


typedef struct {
    int16_t x, y;
} ScreenPosition;

typedef struct {
    Sprite* pSprite;
    ScreenPosition* position;
} ScreenEntity;

// Sprite Procedures
void Sprite_Load_static(Sprite* pSprite, const char* file) {
    printf("\nSprite_load from %s.", file);
//    pSprite->pImage = SDL_LoadBMP(file);
    pSprite->pImage = Framework_LoadBMPFile(file);

    if (pSprite->pImage == NULL) {
        printf("\nError loading Sprite \"%s\" : %s", file, SDL_GetError());
        Framework_ShutDown();
//        SDL_Quit();
        exit(1);
    }

    pSprite->Rectangle.x = 0;
    pSprite->Rectangle.y = 0;
    pSprite->Rectangle.w = pSprite->pImage->w;
    pSprite->Rectangle.h = pSprite->pImage->h;
    printf("\nValues for %s for pSprite->Rectangle: %d, %d", file, pSprite->Rectangle.w, pSprite->Rectangle.h);
}

void Sprite_Load_frame(Sprite* pSprite, const char* sFilename, uint8_t NumFrames, uint16_t FrameWidth,
        uint16_t FrameHeight) {
    printf("\nSprite_load from %s (%d,%d,%d)", sFilename, NumFrames, FrameWidth, FrameHeight);
    assert(pSprite);
    Sprite_Load_static(pSprite, sFilename);
    pSprite->NumFramesX = NumFrames;
    pSprite->FrameWidth = FrameWidth;
    pSprite->FrameHeight = FrameHeight;
    pSprite->FrameRectangle.w = FrameWidth;
    pSprite->FrameRectangle.h = FrameHeight;
    pSprite->NumFramesX = pSprite->pImage->w / pSprite->FrameWidth;
}

void Sprite_SetColorKey(Sprite* pSprite, uint8_t R, uint8_t G, uint8_t B) {
    OpenAsteroids_SpriteSetColorKey(pSprite, R, G, B);
//    SDL_SetColorKey(pSprite->pImage, SDL_SRCCOLORKEY, SDL_MapRGB(pSprite->pImage->format, R, G, B));
}

void Sprite_SetPos(Sprite* pSprite, float fXPos, float fYPos) {
    pSprite->Rectangle.x = (int) (fXPos);
    pSprite->Rectangle.y = (int) (fYPos);
}

void Sprite_Render_static(Sprite* pSprite) {
    OpenAsteroids_RenderWholeSprite(pSprite);
//    SDL_BlitSurface(pSprite->pImage, NULL, FRAMEWORK.pScreen, &pSprite->Rectangle);
}

void Sprite_Render_frame(Sprite* pSprite, float fFrameNumber) {
    // printf("\nstart render frame %d", (int)fFrameNumber);    // will be called on ANY game-frame-run - better put in comments ;)
    int Column = (int) (fFrameNumber) % pSprite->NumFramesX;
    int Row = (int) (fFrameNumber) / pSprite->NumFramesX;
    pSprite->FrameRectangle.x = Column * pSprite->FrameWidth;
    pSprite->FrameRectangle.y = Row * pSprite->FrameHeight;
    //SDL_BlitSurface(pSprite->pImage, &pSprite->FrameRectangle, FRAMEWORK.pScreen, &pSprite->Rectangle);
    OpenAsteroids_BlitSprite(pSprite);
}

void Game_SetRectsForPoints(SDL_Rect* aNumbers[], size_t numberOfRects, size_t xMaxColumns) {
    int i, yPos, xPos, rowStep = 32, columnStep = 24;
    for (i = 0, xPos = 0, yPos = 0; i <= numberOfRects; i++, xPos += columnStep) {
        if (xPos == (columnStep * xMaxColumns)) {
            printf("\nfound xPos=%d,yPos=%d at i=%d", xPos, yPos, i);
            xPos = 0;
            yPos = yPos + rowStep;
            printf("\nreset xPos=%d,yPos=%d", xPos, yPos);
        }
        aNumbers[i]->x = 0 + xPos;
        aNumbers[i]->y = 0 + yPos;
        aNumbers[i]->h = 32;
        aNumbers[i]->w = 24;
    }
    printf("\nfinally aNumber contains %d entries", i);
}

// Text Processing Definitions
struct TEXT {
    Sprite* pSpriteText;
//    SDL_Rect* pRectTextStartSource;
//    SDL_Rect* pRectTextStartDestiny;
    float fXPos;                 // X-Position des Textes
    float fYPos;                   // Y-Position des Textes
    float fAnimPhase;
    SDL_Rect rect;
};
typedef struct TEXT Text;
void Text_Init(Text* pText) {
    pText->pSpriteText = calloc(1, sizeof(Sprite));
    assert(pText->pSpriteText);
    Sprite_Load_frame(pText->pSpriteText, "Data/openAsteroidsTexts.bmp", 2, 640, 128);
    Sprite_SetColorKey(pText->pSpriteText, 255, 0, 255);
    pText->fAnimPhase = 0.0f;
}

void Text_RenderStart(Text* pText) {
    Sprite_SetPos(pText->pSpriteText, 75, 225);
    Sprite_Render_frame(pText->pSpriteText, pText->fAnimPhase);
}

void Text_RenderEnd(Text* pText) {
    pText->fAnimPhase = 1.0f;
    Sprite_SetPos(pText->pSpriteText, 100, 225);
    Sprite_Render_frame(pText->pSpriteText, pText->fAnimPhase);
}

// Explosion Definitions
typedef struct {
    Sprite* pSpriteExplosion;
    float fXPos;                  // X-Position der Explosion
    float fYPos;                  // Y-Position der Explosion
    float fAnimPhase;
    float fExplosionTimerFade;
    int bIsActive;              // "Lebt" der Asteroid noch?
    SDL_Rect Rect;
} Explosion;
void Explosion_Init(Explosion* pExplosion, Sprite *pSpriteExplosion, float fXPos, float fYPos) {
    //printf("\nExplosion init on time(%f) at Pos %f, %f", TIMER.fElapsed, fXPos, fYPos);
    pExplosion->pSpriteExplosion = calloc(1, sizeof(Sprite*));
    assert(pExplosion->pSpriteExplosion && pSpriteExplosion);
    pExplosion->pSpriteExplosion = pSpriteExplosion;

    pExplosion->fXPos = fXPos;
    pExplosion->fYPos = fYPos;
    // Animation starts with 1st frame
    pExplosion->fAnimPhase = 0.0f;

    pExplosion->Rect.x = (int) (fXPos);
    pExplosion->Rect.y = (int) (fYPos);
    //printf("\nInit pSpriteExplosion->FrameRect.w , h : %d, %d", pSpriteExplosion->FrameRect.w, pSpriteExplosion->FrameRect.h);
    pExplosion->Rect.w = pSpriteExplosion->FrameRectangle.w;
    pExplosion->Rect.h = pSpriteExplosion->FrameRectangle.h;

    pExplosion->bIsActive = 1;
    pExplosion->fExplosionTimerFade = 0.0f;
}

void Explosion_Render(Explosion* pExplosion) {
    assert((pExplosion) && (pExplosion->pSpriteExplosion));
    if (pExplosion->bIsActive == 1) {
        Sprite_SetPos(pExplosion->pSpriteExplosion, pExplosion->fXPos, pExplosion->fYPos);
        Sprite_Render_frame(pExplosion->pSpriteExplosion, pExplosion->fAnimPhase);
    }
}

void Explosion_Update(Explosion* pExplosion) {
    assert(pExplosion);
    if (pExplosion->bIsActive == 1) {
        pExplosion->fYPos += 100.0f * TIMER.fElapsed;
        pExplosion->fAnimPhase += 15.0f * TIMER.fElapsed;
        Explosion_Render(pExplosion);

        if (pExplosion->fAnimPhase >= 7.0f) {
            pExplosion->bIsActive = 0;
        }
        if (pExplosion->fYPos > 590.0f)
            pExplosion->bIsActive = 0;
    }
}

// Asteroids Definitions
typedef struct {
    Sprite* pSpriteAsteroid;    // Zeiger auf Asteroiden-Sprite
    Sprite* pSpriteExplosion;
    Explosion* pExplosion;
    float fXPos;                // X-Position des Asteroiden
    float fYPos;                // Y-Position des Asteroiden
    float fAnimPhase;           // Akt. Animationsphase des Asteroiden
    int bIsAlive;               // "Lebt" der Asteroid noch?
    int bGotShot;
    SDL_Rect rectangle;                    // Rect des Asteroiden
} Asteroid;
void Asteroid_Init(Asteroid* pAsteroid, Sprite* pSpriteAsteroid, Sprite* pSpriteExplosion, float fXPos, float fYPos) {
    pAsteroid->pSpriteAsteroid = pSpriteAsteroid;
    pAsteroid->pSpriteExplosion = pSpriteExplosion;
    assert(pAsteroid->pSpriteExplosion);
    assert(pAsteroid->pSpriteAsteroid);

    pAsteroid->fXPos = fXPos;
    pAsteroid->fYPos = fYPos;
    pAsteroid->fAnimPhase = 0.0f;       // Animation starts with 1st frame

    pAsteroid->rectangle.x = (int) (fXPos);
    pAsteroid->rectangle.y = (int) (fYPos);
    // printf("\npSpriteAsteroid->Rect.w , h : %d, %d", pSpriteAsteroid->Rect.w, pSpriteAsteroid->Rect.h);
    pAsteroid->rectangle.w = pSpriteAsteroid->Rectangle.w;
    pAsteroid->rectangle.h = pSpriteAsteroid->Rectangle.h;

    pAsteroid->bIsAlive = 1;
    pAsteroid->bGotShot = 0;
}

void Asteroid_Render(Asteroid* pAsteroid) {
    assert((pAsteroid) && (pAsteroid->pSpriteAsteroid));
    if (pAsteroid->bIsAlive == 1) {
        Sprite_SetPos(pAsteroid->pSpriteAsteroid, pAsteroid->fXPos, pAsteroid->fYPos);
        Sprite_Render_frame(pAsteroid->pSpriteAsteroid, pAsteroid->fAnimPhase);
    }
}

float randomize(int min, int max) {
    return (((rand() % (max * 100)) + (min * 100)) / 100.0f);
}
void Asteroid_Update(Asteroid* pAsteroid) {
    // move Asteroid downwards & calculate animation frame
    if (pAsteroid->bIsAlive == 1) {
        //pAsteroid->fYPos += 100.0f * TIMER.fElapsed;
        pAsteroid->fYPos += randomize(1, 300) * TIMER.fElapsed;
        pAsteroid->rectangle.y = (int) (pAsteroid->fYPos);
        pAsteroid->fAnimPhase += 10.0f * TIMER.fElapsed;
        if (pAsteroid->fAnimPhase >= 10.0f)
            pAsteroid->fAnimPhase = 0.0f;
        if (pAsteroid->fYPos > 590.0f) {
            pAsteroid->bIsAlive = 0;
            // printf("\naAsteroid[i](%p) below screen", pAsteroid);
        }
    }
    if (pAsteroid->bGotShot == 1) {
        pAsteroid->fYPos += randomize(1, 300) * TIMER.fElapsed;
        pAsteroid->rectangle.y = (int) (pAsteroid->fYPos);
        pAsteroid->fAnimPhase += 10.0f * TIMER.fElapsed;
        if (pAsteroid->fAnimPhase >= 15.0f) {
            pAsteroid->bIsAlive = 0;
        }
        if (pAsteroid->fYPos > 590.0f) {
            pAsteroid->bIsAlive = 0;
            // printf("\naAsteroid[i](%p) below screen", pAsteroid);
        }
    }
}



// Shot Definitions
typedef struct {
    Sprite* pSpriteShot;    // Zeiger auf Laser-Sprite
    float fXPos;              // X-Position des Schusses
    float fYPos;              // Y-Position des Schusses
    int bIsAlive;               // "Lebt" der Schuss noch?
    SDL_Rect Rect;               // Rect des Schusses
} Shot;
void Shot_Init(Shot* pShot, Sprite* pSpriteShot, float fXPos, float fYPos) {
    // printf("\nShot_Init with pShot(%p) at Pos %f, %f", pShot, fXPos, fYPos);
    assert(pShot && pSpriteShot);
    pShot->pSpriteShot = pSpriteShot;
    pShot->fXPos = fXPos;
    pShot->fYPos = fYPos;

    pShot->Rect.x = (int) (fXPos);
    pShot->Rect.y = (int) (fYPos);
    pShot->Rect.w = pSpriteShot->Rectangle.w;
    pShot->Rect.h = pSpriteShot->Rectangle.h;
    pShot->bIsAlive = 1;
}

void Shot_Render(Shot* pShot) {
    if (pShot->bIsAlive != 0) {
        Sprite_SetPos(pShot->pSpriteShot, pShot->fXPos, pShot->fYPos);
        Sprite_Render_static(pShot->pSpriteShot);
    }
}

void Shot_Update(Shot* pShot) {
    // assert(pShot->bIsAlive == 1);        // showed, that shots already dead still got updatet - SKANDAL!
    if (pShot->bIsAlive) {
        pShot->fYPos -= 400.0f * TIMER.fElapsed;
        pShot->Rect.y = (int) (pShot->fYPos);
        if (pShot->fYPos < -15.0f) {        // deactivate Shot if above screen
            pShot->bIsAlive = 0;
        }
    }
}
// Player Defintiions
typedef struct {
    Sprite* pSpritePlayer;  // Sprite f�r Spieler
    Sprite* pSpriteShot;                      // Sprite f�r Lasersch�sse
    Sprite* pSpriteExplosion;
    Explosion* pExplosion;
    float fXPos;                  // X-Position des Spielers
    float fYPos;                  // Y-Position des Spielers
    float fMousePos;
    float fAnimPhase;             // Aktuelle Animationsphase
    int bMousePressed;
    int bShotLock;
    int bIsAlive;
    int iPoints;
// simulate linked list with an array
    Shot* aShots[40];
    int iaShots;
    int iMaxShots;
} Player;

void Player_Init(Player* pPlayer, Sprite* pSpriteExplosion) {
    printf("\nPlayer init");
    pPlayer->pSpritePlayer = calloc(1, sizeof(Sprite));
    Sprite_Load_frame(pPlayer->pSpritePlayer, "Data/Player.bmp", 11, 64, 64);
    Sprite_SetColorKey(pPlayer->pSpritePlayer, 255, 0, 255);
    printf("\nloaded player data");

    pPlayer->pSpriteShot = calloc(1, sizeof(Sprite));
    Sprite_Load_static(pPlayer->pSpriteShot, "Data/Shot.bmp");
    Sprite_SetColorKey(pPlayer->pSpriteShot, 255, 0, 255);
    pPlayer->iMaxShots = 40;
    pPlayer->bIsAlive = 1;

    pPlayer->pSpriteExplosion = calloc(1, sizeof(Sprite));
    pPlayer->pSpriteExplosion = pSpriteExplosion;
    pPlayer->pExplosion = calloc(1, sizeof(Explosion));

    int i = -1;
    while (++i < pPlayer->iMaxShots) {
        pPlayer->aShots[i] = calloc(1, sizeof(Shot));
    }
    assert(pPlayer->pSpritePlayer && pPlayer->pSpriteShot && pPlayer->pSpriteShot);
    assert(pPlayer->pExplosion->bIsActive == 0);
}

void Player_Reset(Player* pPlayer) {
    pPlayer->fXPos = 376.0f;
    pPlayer->fMousePos = 376.0f;
    pPlayer->fYPos = 520.0f;
    pPlayer->fAnimPhase = 5.0f;
    pPlayer->iaShots = 0;
    pPlayer->bShotLock = 1;
    pPlayer->bMousePressed = 0;
    pPlayer->bIsAlive = 1;
    pPlayer->iPoints = 0;
    // printf("\nreseting Player Pos(%f, %f)", pPlayer->fXPos, pPlayer->fYPos);
}

void Player_ProcessMoving(Player* pPlayer) {
    if (Framework_KeyDown(SDLK_LEFT)) {
        pPlayer->fXPos -= 300.0f * TIMER.fElapsed;  // move left
        pPlayer->fAnimPhase -= 20.0f * TIMER.fElapsed;
    } else if (Framework_KeyDown(SDLK_RIGHT)) {
        pPlayer->fXPos += 300.0f * TIMER.fElapsed;  // move right
        pPlayer->fAnimPhase += 20.0f * TIMER.fElapsed;
    } else {    // when Player was not moved furthermore, go to central animation
        if (pPlayer->fAnimPhase > 5.0f)
            pPlayer->fAnimPhase -= 20.0f * TIMER.fElapsed;
        if (pPlayer->fAnimPhase < 5.0f)
            pPlayer->fAnimPhase += 20.0f * TIMER.fElapsed;
    }
}

void Player_ProcessShooting(Player* pPlayer) {
    if (pPlayer->iaShots == pPlayer->iMaxShots - 1)
        pPlayer->iaShots = 0;

    if ((pPlayer->bShotLock == 0) && (Framework_KeyDown(SDLK_SPACE))) {
        assert(pPlayer->iaShots < pPlayer->iMaxShots);  // make it go crash
        // looking for "free" slot
        int i;
        for (i = 0; i < pPlayer->iMaxShots; i++) {
            if (pPlayer->aShots[i]->bIsAlive == 0) {
                pPlayer->iaShots = i;
                break;
            }
            if (pPlayer->iaShots == (pPlayer->iMaxShots - 1))
                pPlayer->iaShots = 0;
        }
        assert(pPlayer->aShots[pPlayer->iaShots]);

        float ShotStart = pPlayer->fXPos + ((pPlayer->pSpritePlayer->Rectangle.w / 2) - 4);
        Shot_Init(pPlayer->aShots[pPlayer->iaShots], pPlayer->pSpriteShot, ShotStart, pPlayer->fYPos);

        pPlayer->iaShots++;
        pPlayer->bShotLock = 1;
    }
    // Shooting allowed AFTER releasing Space
    if (Framework_KeyDown(SDLK_SPACE) == 0)
        pPlayer->bShotLock = 0;
}

void Player_CheckPosition(Player* pPlayer) {
    if (pPlayer->fXPos < 0.0f)
        pPlayer->fXPos = 0.0f;
    else if (pPlayer->fXPos > 752.0f)
        pPlayer->fXPos = 752.0f;
    if (pPlayer->fAnimPhase < 0.0f)
        pPlayer->fAnimPhase = 0.0f;
    else if (pPlayer->fAnimPhase > 10.0f)
        pPlayer->fAnimPhase = 10.0f;
}
void Player_Update(Player* pPlayer) {
    // 2nd: react on Keyboard-Input
    Player_ProcessMoving(pPlayer);
    // 1st: check current player Position
    Player_CheckPosition(pPlayer);
    // 3rd: the shooting
    Player_ProcessShooting(pPlayer);
}



void Player_Render(Player* pPlayer) {
    Sprite_SetPos(pPlayer->pSpritePlayer, pPlayer->fXPos, pPlayer->fYPos);
    Sprite_Render_frame(pPlayer->pSpritePlayer, pPlayer->fAnimPhase);
    // assert(pPlayer->pShot);      // put in comments otherwise failes after 1st shot gone
    int i;
    for (i = 0; i < pPlayer->iMaxShots; i++) {
        if (pPlayer->aShots[i]) {
            Shot_Update(pPlayer->aShots[i]);
            if (pPlayer->aShots[i]->bIsAlive == 1) {
                Shot_Render(pPlayer->aShots[i]);
            }
        }
    }
}

void Player_RenderExplosion(Player* pPlayer) {
    assert(pPlayer->pExplosion);
    if (pPlayer->pExplosion->bIsActive == 1) {
        Explosion_Update(pPlayer->pExplosion);
        Explosion_Render(pPlayer->pExplosion);
    }
}

// Game Score Definitions
typedef struct {

} EntityScore;

// Game Constants
const uint8_t SCORE_NUMBERS;

// Game Definitions
typedef struct {
    Sprite* pSpriteBackground;
    Sprite* pSpriteAsteroid;
    Sprite* pSpriteExplosion;
    Sprite* pSpriteText;
    Sprite* pSpriteScores;
    SDL_Rect* aNumbers[11];
    //SDL_Rect* aDisplayNumbers[SCORE_NUMBERS];
    SDL_Rect* aDisplayNumbers[6];
    //SDL_Rect* aDisplayNumbers[6]; // works
    SDL_Rect* pRectClippingArea;
    SDL_Rect* pRectClippingSource;
    float fBackgroundPosY;
    Player* pPlayer;
    Text* pText;
    float fAsteroidTimer;
    int bGameRun;
    int bGameStatus;
    int bGameReady;
// simulate linked list with an array
    Asteroid* aAsteroids[20];
    int iaAsteroids;
    int iMaxAsteroids;
} Game;

// give value
const uint8_t SCORE_NUMBERS = 6;

void Sprite_InitBackground(Sprite* pSpriteBackground) {
    pSpriteBackground->Rectangle.x = 0;
    pSpriteBackground->Rectangle.y = 1200;
    pSpriteBackground->Rectangle.w = 800;
    pSpriteBackground->Rectangle.h = 600;
//    pSpriteBackground->Clip.x = 0;
//    pSpriteBackground->Clip.y = 0;
//    pSpriteBackground->Clip.w = FRAMEWORK.pScreen->w;
//    pSpriteBackground->Clip.h = FRAMEWORK.pScreen->h;
//    assert(pSpriteBackground->Clip.w == 800);
}
void Game_Init(Game* pGame) {
    printf("\nstart Game_init");
    srand(time(0));

    pGame->pPlayer = calloc(1, sizeof(Player));
    pGame->pText = calloc(1, sizeof(Text));

    printf("\ninit player and text");
    Player_Init(pGame->pPlayer, pGame->pSpriteExplosion);
    Player_Reset(pGame->pPlayer);
    Text_Init(pGame->pText);

    pGame->pSpriteAsteroid = calloc(1, sizeof(Sprite));
    pGame->pSpriteExplosion = calloc(1, sizeof(Sprite));
    pGame->pSpriteBackground = calloc(1, sizeof(Sprite));
    pGame->pSpriteScores = calloc(1, sizeof(Sprite));
    printf("\nallocate asteroids and so on");
    int a;
    for (a = 0; a <= 11; a++) {
        pGame->aNumbers[a] = calloc(1, sizeof(SDL_Rect));
    }
    // init array
    //pGame->aDisplayNumbers = calloc(SCORE_NUMBERS,sizeof(SDL_Rect));
    for (a = 0; a < SCORE_NUMBERS; a++) {
        pGame->aDisplayNumbers[a] = calloc(1, sizeof(SDL_Rect));
    }
    Game_SetRectsForPoints(pGame->aNumbers, 11, 10);

    assert(pGame->pPlayer && pGame->pSpriteAsteroid);
    assert((sizeof(pGame->aAsteroids) / sizeof(Asteroid*)) == 20);

    printf("\nloading background");
    Sprite_Load_static(pGame->pSpriteBackground, "Data/SimpleScrollingSpace.bmp");
    Sprite_InitBackground(pGame->pSpriteBackground);
    assert(pGame->pSpriteBackground->Rectangle.h == GAME_WINDOW_HEIGHT);

    Sprite_Load_frame(pGame->pSpriteAsteroid, "Data/RockMedium.bmp", 15, 38, 38);
    Sprite_SetColorKey(pGame->pSpriteAsteroid, 255, 0, 255);
    Sprite_Load_frame(pGame->pSpriteExplosion, "Data/Explosion.bmp", 7, 110, 110);
    Sprite_SetColorKey(pGame->pSpriteExplosion, 255, 0, 255);
    Sprite_Load_frame(pGame->pSpriteScores, "Data/Numbers.bmp", 10, 24, 32);
    Sprite_SetColorKey(pGame->pSpriteScores, 255, 0, 255);

    pGame->fAsteroidTimer = 0.0f;
    pGame->bGameRun = 0;
    pGame->bGameReady = 1;
    pGame->bGameStatus = 1;
    pGame->fBackgroundPosY = 0.0f;
    pGame->iaAsteroids = 0;
    pGame->iMaxAsteroids = 20;

    int i = -1;
    while (++i < pGame->iMaxAsteroids) {
        pGame->aAsteroids[i] = calloc(1, sizeof(Asteroid));
        pGame->aAsteroids[i]->pExplosion = calloc(1, sizeof(Explosion));
    }
    assert(pGame->pPlayer && pGame->pSpriteAsteroid && pGame->pSpriteExplosion);
}

void Game_ProcessEvents(Game* pGame) {
    SDL_Event Event;
    if (SDL_PollEvent(&Event)) {
//        Uint8 type = Event.type;
//        printf("\nencountered SDL_Event of type %d.",type);

        switch (Event.type) {
        case (SDL_QUIT):
            pGame->bGameRun = 0;
            pGame->bGameReady = 0;
            break;
        case (SDL_KEYDOWN):
            switch (Event.key.keysym.sym) {
            case (SDLK_SPACE):
                pGame->bGameRun = 1;
                break;
            case (SDLK_c):
                pGame->pPlayer->bIsAlive = 1;
                pGame->bGameStatus = 1;
                break;
            case (SDLK_ESCAPE):
                pGame->bGameRun = 0;
                pGame->bGameReady = 0;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void Game_SpawnAsteroids(Game* pGame, Asteroid* aAsteroids[]) {
    pGame->fAsteroidTimer += TIMER.fElapsed;
    if (pGame->fAsteroidTimer >= 0.75f) {
        int i;
        for (i = 0; i < pGame->iMaxAsteroids; i++) {
            if (aAsteroids[i]->bIsAlive == 0) {
                pGame->iaAsteroids = i;
                break;
            }
            if (pGame->iaAsteroids == (pGame->iMaxAsteroids - 1))
                pGame->iaAsteroids = 0;
        }
        assert(aAsteroids[pGame->iaAsteroids]);
        assert(pGame->pSpriteAsteroid);

        int XPos = rand() % 736;
        Asteroid_Init(aAsteroids[pGame->iaAsteroids], pGame->pSpriteAsteroid, pGame->pSpriteExplosion, (float) XPos,
                -40.0f);

        // workaround for 1st Asteroids Rect-Bug
        // assert(aAsteroids[pGame->iaAsteroids]->pSpriteAsteroid->FrameWidth == 38);
        if (aAsteroids[pGame->iaAsteroids]->rectangle.w > aAsteroids[pGame->iaAsteroids]->pSpriteAsteroid->FrameWidth) {
            aAsteroids[pGame->iaAsteroids]->rectangle.w = aAsteroids[pGame->iaAsteroids]->pSpriteAsteroid->FrameWidth;
            aAsteroids[pGame->iaAsteroids]->rectangle.h = aAsteroids[pGame->iaAsteroids]->pSpriteAsteroid->FrameHeight;
        }

        aAsteroids[pGame->iaAsteroids]->bIsAlive = 1;
        pGame->fAsteroidTimer = 0.0f;
        pGame->iaAsteroids++;
    }
}

int Game_RectsCollide(SDL_Rect RectA, SDL_Rect RectB, unsigned short int* bounds) {
    if (bounds == NULL)
        fprintf(stderr, "\nIllegal Value for 3.Argument in Game_Rects_Collide");

    if ((RectA.y < RectB.y + RectB.h) && (RectA.y + RectA.h > RectB.y - bounds[2])
            && (RectA.x < RectB.x + RectB.w - bounds[1]) && (RectA.x + RectA.w > RectB.x + bounds[3]))
        return (1);
    else
        return (0);
}

/************************************************************************
 Game_CheckCollisions
 a collision is the case, when 2 Rects do overlapp
 1) y of Lower Bound of Rect A is smaller than Upper Bound of Rect B (RectA.y < RectB.y + RectB.hight)
 2) y of Upper Bound of Rect A is greater than Lower Bound of Rect B (RectA.y + RectA.height > RectB.y)
 3) x of Lower Left of Rect A is smaller than Lower Right of Rect B (RectA.x < RectB.x + RectB.width)
 4) x of Lower Right of Rect A is greater than Lower Left of Rect B(RectA.x + RectA.width > RectB.x)

 ************************************************************************/

void Game_CheckCollisions(Game* pGame) {
    SDL_Rect RectAsteroid;
    SDL_Rect RectShot;
    SDL_Rect RectPlayer;
    unsigned short int boundsAsteroid[4] = { 0, 5, 5, 5 };
    int h, i;

    // Asteroid with Shot
    for (h = 0; h < pGame->iMaxAsteroids; h++) {
        assert(pGame->aAsteroids[h]);
        if (pGame->aAsteroids[h]->bIsAlive == 1) {
            RectAsteroid = pGame->aAsteroids[h]->rectangle;
        } else
            continue;
        for (i = 0; i < pGame->pPlayer->iMaxShots; i++) {
            if (pGame->pPlayer->aShots[i] && pGame->pPlayer->aShots[i]->bIsAlive == 1)
                RectShot = pGame->pPlayer->aShots[i]->Rect;
            else
                continue;

            if (Game_RectsCollide(RectShot, RectAsteroid, boundsAsteroid) == 1) {
                pGame->pPlayer->aShots[i]->bIsAlive = 0;
                pGame->aAsteroids[h]->bIsAlive = 0;
                // sound-thread
                //SDL_Thread* soundThread = calloc(1, sizeof(SDL_Thread*));
                //soundThread = SDL_CreateThread(Sound_Explosion_Wrapper, "Data/Explosion.wav");
                pGame->pPlayer->iPoints += 1;
                if (pGame->aAsteroids[h]->pExplosion->bIsActive == 0) {
                    Explosion_Init(pGame->aAsteroids[h]->pExplosion, pGame->pSpriteExplosion,
                            (float) pGame->aAsteroids[h]->fXPos, (float) pGame->aAsteroids[h]->fYPos);
                    assert(pGame->aAsteroids[h]->pExplosion->bIsActive == 1);
                }
            }
        }
    }

    // Player with Asteroid
    if (pGame->pPlayer->bIsAlive == 1) {
        RectPlayer = pGame->pPlayer->pSpritePlayer->Rectangle;

        for (h = 0; h < pGame->iMaxAsteroids; h++) {
            if (pGame->aAsteroids[h] && pGame->aAsteroids[h]->bIsAlive == 1)
                RectAsteroid = pGame->aAsteroids[h]->rectangle;
            else
                continue;
            if (Game_RectsCollide(RectPlayer, RectAsteroid, boundsAsteroid) == 1) {
                assert(pGame->pPlayer->bIsAlive == 1);
                pGame->aAsteroids[h]->bIsAlive = 0;
                pGame->pPlayer->bIsAlive = 0;
                Explosion_Init(pGame->pPlayer->pExplosion, pGame->pSpriteExplosion, (float) pGame->pPlayer->fXPos,
                        (float) pGame->pPlayer->fYPos);
                assert(pGame->pPlayer->pExplosion->bIsActive == 1);
            }
        }
    }
}

void Game_RenderAsteroids(Game* pGame, Asteroid* aAsteroids[]) {
    assert(pGame->pSpriteAsteroid);

    if (pGame->iaAsteroids > 0) {
        int i = -1;
        while (++i < pGame->iMaxAsteroids) {
            if (aAsteroids[i]) {
                if (aAsteroids[i]->bIsAlive == 1) {
                    Asteroid_Update(aAsteroids[i]);
                    Asteroid_Render(aAsteroids[i]);
                } else {
                    continue;
                }
            } else
                continue;
        }
    }
}

void Game_RenderAsteroidsExplosions(Game* pGame, Asteroid* aAsteroids[]) {
    if (pGame->iaAsteroids > 0) {
        int i = -1;
        while (++i < pGame->iMaxAsteroids) {
            if (aAsteroids[i]->pExplosion->bIsActive == 1) {
                Explosion_Update(aAsteroids[i]->pExplosion);
                Explosion_Render(aAsteroids[i]->pExplosion);
            } else {
                continue;
            }
        }
    }
}

void Game_RenderPoints(Game* pGame, int iPoints) {
//    printf("\nrendering %d points", iPoints);

    int i, j;
    char punkte[SCORE_NUMBERS];

    sprintf(punkte, "%06d", iPoints);
//    printf("\nformatted as %s points", punkte);

    for (i = 0, j = 0; i < SCORE_NUMBERS; i++, j += 24) {
        pGame->aDisplayNumbers[i]->x = 700 - j;
        pGame->aDisplayNumbers[i]->y = 50;
        pGame->aDisplayNumbers[i]->h = 32;
        pGame->aDisplayNumbers[i]->w = 24;

        int tmp = 0;
        tmp = (punkte[SCORE_NUMBERS - (1 + i)]) - 48;
//        printf("\ntmp = %d",tmp);

        SDL_BlitSurface(pGame->pSpriteScores->pImage, pGame->aNumbers[tmp + 1], FRAMEWORK.pScreen,
                pGame->aDisplayNumbers[i]);
//        OpenAsteroids_BlitSpriteFramePart(pGame, pGame->pSpriteNumbers, pGame->aNumbers[tmp + 1],pGame->aDisplayNumbers[i]);
    }
}

void Game_UpdateBackground(Game* pGame, Sprite* pSpriteBackground) {

    pGame->fBackgroundPosY += 50.0f * TIMER.fElapsed;
    if (pGame->fBackgroundPosY > 1.0f) {
        pSpriteBackground->Rectangle.y -= (int) pGame->fBackgroundPosY;
        pGame->fBackgroundPosY = 0.0f;
    }
    if (pSpriteBackground->Rectangle.y < 1)
        pSpriteBackground->Rectangle.y = 1200;
}

void Game_RenderBackground(Sprite* pSpriteBackground) {
    assert(pSpriteBackground->pImage);
    assert(pSpriteBackground->Rectangle.x == 0);

    if (SDL_BlitSurface(pSpriteBackground->pImage, &pSpriteBackground->Rectangle, FRAMEWORK.pScreen,
            &pSpriteBackground->Clip) != 0) {
//        &pSpriteBackground->Clip) != 0) {
        fprintf(stderr, "\nBLIT BACKGROUND: %s", SDL_GetError());
        exit(0);
    }
}
void Game_Run(Game* pGame, Player* pPlayer, SDL_Surface* pScreen) {
    assert(pGame && pPlayer);

    while (pGame->bGameReady == 1) {
        Game_ProcessEvents(pGame);
        Framework_Update();
        Framework_Clear();

        OpenAsteroids_fillScreenWithBlack();
//        SDL_FillRect(pScreen, NULL, SDL_MapRGB(pScreen->format, 0, 0, 0));

        Game_RenderBackground(pGame->pSpriteBackground);

        if (pGame->bGameStatus == 1) {
            Text_RenderStart(pGame->pText);
            pPlayer->bShotLock = 1;
        }

        if (pGame->pPlayer->bIsAlive == 1) {
            Player_Update(pPlayer);
            Player_Render(pPlayer);
        }

        if (pGame->bGameRun == 1) {
            pGame->bGameStatus = 0;
            Game_SpawnAsteroids(pGame, pGame->aAsteroids);
            Game_CheckCollisions(pGame);
            Game_UpdateBackground(pGame, pGame->pSpriteBackground);
            Game_RenderAsteroids(pGame, pGame->aAsteroids);
            Game_RenderAsteroidsExplosions(pGame, pGame->aAsteroids);

            Game_RenderPoints(pGame, pPlayer->iPoints);
        }

        if (pPlayer->bIsAlive == 0) {
            Player_RenderExplosion(pPlayer);
            pPlayer->iPoints = 0;
            if (pPlayer->pExplosion->bIsActive == 0) {
                Text_RenderEnd(pGame->pText);
            }
        }

        // copy Backbuffer to Screenbuffer
        OpenAsteroids_FlipSCreenBuffer();
//        SDL_Flip(pScreen);
    }
}


#endif /* SRC_OPENASTEROIDS_H_ */
