#include <stdio.h>
#include "OpenAsteroids.h"

int main(int argc, char *argv[]) {
    SDL_version compiled;
    const SDL_version* linked = SDL_Linked_Version();
    SDL_VERSION(&compiled);
    printf("compiled SDL version %d.%d.%d.\n", compiled.major, compiled.minor, compiled.patch);
    printf("linked   SDL version %d.%d.%d.\n", linked->major, linked->minor, linked->patch);

    printf("\ninit framework ...");
    Framework_Init();
    printf("\nframework inited");

    Game* game = (Game*) calloc(1, sizeof(Game));
    Game_Init(game);
    printf("\ninit game");
    
    Game_Run(game, game->pPlayer, FRAMEWORK.pScreen);

    printf("\nquit game ... ");
    Framework_Quit();
    printf("\nbye!");
    
    return (0);
}
