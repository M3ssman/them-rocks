#include "OpenAsteroids.h"

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
