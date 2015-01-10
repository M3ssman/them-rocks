#include "OpenAsteroids.h"

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
    // assert(pShot->bIsAlive == 1);		// showed, that shots already dead still got updatet - SKANDAL!
    if (pShot->bIsAlive) {
        pShot->fYPos -= 400.0f * TIMER.fElapsed;
        pShot->Rect.y = (int) (pShot->fYPos);
        if (pShot->fYPos < -15.0f) {		// deactivate Shot if above screen
            pShot->bIsAlive = 0;
        }
    }
}
