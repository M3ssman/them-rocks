/*
 * OpenAsteriods.c
 *
 *  Created on: 06.01.2015
 *      Author: u_hartwig
 */

#include "OpenAsteroids.h"

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
