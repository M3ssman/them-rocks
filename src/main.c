#include <stdio.h>
#include "OpenAsteroids.h"

int main(int argc, char *argv[]) {
    // welche sdl versionen wurden verwendet
    SDL_version compiled;
    const SDL_version* linked = SDL_Linked_Version();
    SDL_VERSION(&compiled);
    printf("kompilierte SDL version %d.%d.%d.\n", compiled.major, compiled.minor, compiled.patch);
    printf("verlinkte SDL version %d.%d.%d.\n", linked->major, linked->minor, linked->patch);

    printf("\nstart at main");

    Framework_Init();
    printf("\ninited framework");

    Game* test = (Game*) calloc(1, sizeof(Game));
    printf("\nallocated game");
    assert(test);
    Game_Init(test);
    printf("\ninit game");
    Game_Run(test, test->pPlayer, FRAMEWORK.pScreen);
    //Game_Quit(test);

    Framework_Quit();

    return (0);
}
