#include "OpenAsteroids.h"

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

void Player_Update(Player* pPlayer) {
    // 2nd: react on Keyboard-Input
    Player_ProcessMoving(pPlayer);
    // 1st: check current player Position
    Player_CheckPosition(pPlayer);
    // 3rd: the shooting
    Player_ProcessShooting(pPlayer);
}

void Player_ProcessMoving(Player* pPlayer) {
    if (Framework_KeyDown(SDLK_LEFT)) {
        pPlayer->fXPos -= 300.0f * TIMER.fElapsed;	// move left
        pPlayer->fAnimPhase -= 20.0f * TIMER.fElapsed;
    } else if (Framework_KeyDown(SDLK_RIGHT)) {
        pPlayer->fXPos += 300.0f * TIMER.fElapsed;	// move right
        pPlayer->fAnimPhase += 20.0f * TIMER.fElapsed;
    } else {	// when Player was not moved furthermore, go to central animation
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
        assert(pPlayer->iaShots < pPlayer->iMaxShots);	// make it go crash
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

void Player_Render(Player* pPlayer) {
    Sprite_SetPos(pPlayer->pSpritePlayer, pPlayer->fXPos, pPlayer->fYPos);
    Sprite_Render_frame(pPlayer->pSpritePlayer, pPlayer->fAnimPhase);
    // assert(pPlayer->pShot);		// put in comments otherwise failes after 1st shot gone
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
