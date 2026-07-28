//
// Test program for bsp generation
//

#include "../../include/level.h"

int main(void)
{
    vec2_t spawn, exit;
    level_t *lvl = level_create(0);

    if (!lvl) {
        return 1;
    }
    spawn = level_spawn(lvl);
    exit = level_exit(lvl);
    printf("--- Map charateristics ---\n\n");
    printf("width: %d\nheight: %d\nspawn: [%.0f, %.0f]\nexit: [%.0f, %.0f]\n",
        level_width(lvl), level_height(lvl), spawn.x, spawn.y, exit.x, exit.y);
    printf("--- Map ---\n\n");
    level_print_ascii(lvl);
    level_destroy(lvl);
    return 0;
}