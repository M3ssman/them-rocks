#include "OpenAsteroids.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// give value
const uint8_t SCORE_NUMBERS = 6;

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
