#include "OpenAsteroids.h"

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

void Sprite_Load_frame(Sprite* pSprite, const char* sFilename, uint8_t NumFrames, uint16_t FrameWidth, uint16_t FrameHeight) {
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
    OpenAsteroids_SpriteSetColorKey(pSprite,R,G,B);
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
    // printf("\nstart render frame %d", (int)fFrameNumber);	// will be called on ANY game-frame-run - better put in comments ;)
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

