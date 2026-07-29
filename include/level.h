//
// level module
//

#ifndef LEVEL_H
#define LEVEL_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <math.h>
    #include "wolf3d.h"


// Grid cells
typedef enum {
    CELL_FLOOR      = 0,
    CELL_CINDER     = 1,
    CELL_CONCRETE   = 2,
    CELL_STEEL      = 3,
    CELL_HAZARD     = 4,
    CELL_CHEM       = 5,
} cell_t;

// --- Life cycle ---
level_t *level_create(int floor_number);
void     level_destroy(level_t *lvl);

// --- Getters to deal with the grid ---
int      level_width(const level_t *lvl);
int      level_height(const level_t *lvl);
int      level_is_wall(const level_t *lvl, int x, int y);
vec2_t   level_spawn(const level_t *lvl);
vec2_t   level_exit(const level_t *lvl);
int level_tile(const level_t *lvl, int x, int y);

// --- Debug (used by tests/test_bsp.c) ---
void     level_print_ascii(const level_t *lvl);

#endif // LEVEL_H
