#include "OpenAsteroids.h"

void Asteroid_Init(Asteroid* pAsteroid, Sprite* pSpriteAsteroid, Sprite* pSpriteExplosion, float fXPos, float fYPos) {
    pAsteroid->pSpriteAsteroid = pSpriteAsteroid;
    pAsteroid->pSpriteExplosion = pSpriteExplosion;
    assert(pAsteroid->pSpriteExplosion);
    assert(pAsteroid->pSpriteAsteroid);

    pAsteroid->fXPos = fXPos;
    pAsteroid->fYPos = fYPos;
    pAsteroid->fAnimPhase = 0.0f; 		// Animation starts with 1st frame

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

float randomize(int min, int max) {
    return (((rand() % (max * 100)) + (min * 100)) / 100.0f);
}
