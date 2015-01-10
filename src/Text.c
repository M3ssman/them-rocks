#include "OpenAsteroids.h"

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
