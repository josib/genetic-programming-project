#include <stdio.h>
#include <stdlib.h>

#include "graphic.h"
#include "world.h"
#include "species.h"
#include "utils.h"

int main ( int argc, char** argv )
{
    graphic_create(WORLD_WIDTH, WORLD_HEIGHT);
    world_init(40, 60);

    // run the main loop
    int i = 0;
    while (graphic_is_open()) {
        graphic_clear();
        world_display(0); // 0 for base graphic, 1 for basic console, 2 for extended console
        graphic_display();

        world_update();
        i++;
    }
    world_destroy();
    return 0;
}
