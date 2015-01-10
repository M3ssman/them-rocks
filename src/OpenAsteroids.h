/*
 * OpenAsteroids.h
 *
 *  Created on: 06.01.2015
 *      Author: u_hartwig
 */
#include "Framework.h"

#ifndef SRC_OPENASTEROIDS_H_
#define SRC_OPENASTEROIDS_H_

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

typedef struct {
    int16_t x, y;
} ScreenPosition;

typedef struct {
    Sprite* pSprite;
    ScreenPosition* position;
} ScreenEntity;

void Sprite_Load_static(Sprite* pSprite, const char* sFilename);
void Sprite_Load_frame(Sprite* pSprite, const char* sFilename, uint8_t NumFrames, uint16_t FrameWidth,
        uint16_t FrameHeight);
void Sprite_Render_static(Sprite* Sprite);
void Sprite_Render_frame(Sprite* pSprite, float fFrameNumber);
void Sprite_SetPos(Sprite* pSprite, float fXPos, float fYPos);
void Sprite_SetColorKey(Sprite* pSprite, uint8_t R, uint8_t G, uint8_t B);
//void Game_SetRectsForPoints(SDL_Rect* aNumbers[]);
void Game_SetRectsForPoints(SDL_Rect* aNumbers[], size_t numberOfRects, size_t xMaxColumns);

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
void Text_Init(Text* pText);
void Text_RenderStart(Text* pText);
void Text_RenderEnd(Text* pText);

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
void Explosion_Init(Explosion* pExplosion, Sprite* pSpriteExplosion, float fXPos, float fYPos);
void Explosion_Render(Explosion* pExplosion);
void Explosion_Update(Explosion* pExplosion);

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
void Asteroid_Init(Asteroid* pAsteroid, Sprite* pSpriteAsteroid, Sprite* pSpriteExplosion, float fXPos, float fYPos);
void Asteroid_Render(Asteroid* pAsteroid);
void Asteroid_Update(Asteroid* pAsteroid);
float randomize(int min, int max);

// Shot Definitions
typedef struct {
    Sprite* pSpriteShot;    // Zeiger auf Laser-Sprite
    float fXPos;              // X-Position des Schusses
    float fYPos;              // Y-Position des Schusses
    int bIsAlive;               // "Lebt" der Schuss noch?
    SDL_Rect Rect;               // Rect des Schusses
} Shot;
void Shot_Init(Shot* pShot, Sprite* pSpriteShot, float fXPos, float fYPos);
void Shot_Render(Shot* pShot);
void Shot_Update(Shot* pShot);

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

void Player_Init(Player* pPlayer, Sprite* pSpriteExplosion);
void Player_Reset(Player* pPlayer);
void Player_Update(Player* pPlayer);
void Player_Render(Player* pPlayer);
void Player_RenderExplosion(Player* pPlayer);
void Player_CheckPosition(Player* pPlayer);
void Player_ProcessMoving(Player* pPlayer);
void Player_ProcessShooting(Player* pPlayer);

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

void Game_Init(Game* pGame);
void Game_Run(Game* pGame, Player* pPlayer, SDL_Surface* pScreen);
void Game_Quit(Game* pGame);
void Game_ProcessEvents(Game* pGame);
void Game_SpawnAsteroids(Game* pGame, Asteroid* aAsteroids[]);
void Game_RenderAsteroids(Game* pGame, Asteroid* aAsteroids[]);
void Game_RenderAsteroidsExplosions(Game* pGame, Asteroid* aAsteroids[]);
void Game_UpdateBackground(Game* pGame, Sprite* pSpriteBackground);
void Game_RenderPoints(Game* pGame, int iPoints);
void Game_RenderBackground(Sprite* pSpriteBackground);
void Sprite_InitBackground(Sprite* pSpriteBackground);
int Game_RectsCollide(SDL_Rect A, SDL_Rect B, unsigned short int* bounds);

/************************************************************************
 Game_CheckCollisions
 a collision is the case, when 2 Rects do overlapp
 1) y of Lower Bound of Rect A is smaller than Upper Bound of Rect B (RectA.y < RectB.y + RectB.hight)
 2) y of Upper Bound of Rect A is greater than Lower Bound of Rect B (RectA.y + RectA.height > RectB.y)
 3) x of Lower Left of Rect A is smaller than Lower Right of Rect B (RectA.x < RectB.x + RectB.width)
 4) x of Lower Right of Rect A is greater than Lower Left of Rect B(RectA.x + RectA.width > RectB.x)

 ************************************************************************/
void Game_CheckCollisions(Game* pGame);

// forward former SDL calls
void OpenAsteroids_BlitSprite(Sprite* spriteToBlit);
//void OpenAsteroids_BlitSpriteFramePart(pGame->pSpriteNumbers->pImage, pGame->aNumbers[tmp + 1],pGame->aDisplayNumbers[i]);
void OpenAsteroids_RenderWholeSprite(Sprite*);
void OpenAsteroids_SpriteSetColorKey(Sprite*, int, int, int);
void OpenAsteroids_fillScreenWithBlack();
void OpenAsteroids_FlipSCreenBuffer();

#endif /* SRC_OPENASTEROIDS_H_ */
