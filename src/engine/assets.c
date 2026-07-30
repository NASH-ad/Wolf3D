//
// Assets module — implementation
//

#include <stdlib.h>
#include "assets.h"

// Chemin des textures, indexé par cell_t (index 0 = sol → NULL)
static const char *WALL_PATHS[WALL_TEX_COUNT] = {
    NULL,                                    // 0 CELL_FLOOR
    "assets/textures/bunker_cinderblock_64.png",  // 1 CELL_CINDER
    "assets/textures/bunker_concrete_64.png",     // 2 CELL_CONCRETE
    "assets/textures/bunker_steel_64.png",        // 3 CELL_STEEL
    "assets/textures/bunker_hazard_64.png",       // 4 CELL_HAZARD
    "assets/textures/bunker_chemistry_64.png",    // 5 CELL_CHEM
};

assets_t *assets_create(void)
{
    assets_t *a = malloc(sizeof(assets_t));

    if (!a)
        return NULL;
    for (int i = 0; i < WALL_TEX_COUNT; i++)
        a->walls[i] = NULL;
    a->floor = NULL;
    a->ceiling = NULL;
    for (int i = 1; i < WALL_TEX_COUNT; i++) {
        a->walls[i] = sfImage_createFromFile(WALL_PATHS[i]);
        if (!a->walls[i])
            return (assets_destroy(a), NULL);
    }
    a->floor = sfImage_createFromFile("assets/textures/bunker_floor_64.png");
    a->ceiling = sfImage_createFromFile("assets/textures/bunker_ceiling_64.png");
    if (!a->floor || !a->ceiling)
        return (assets_destroy(a), NULL);
    return a;
}

void assets_destroy(assets_t *a)
{
    if (!a)
        return;
    for (int i = 0; i < WALL_TEX_COUNT; i++)
        if (a->walls[i])
            sfImage_destroy(a->walls[i]);
    if (a->floor)
        sfImage_destroy(a->floor);
    if (a->ceiling)
        sfImage_destroy(a->ceiling);
    free(a);
}

const sfImage *assets_wall(const assets_t *a, int cell_type)
{
    if (cell_type <= 0 || cell_type >= WALL_TEX_COUNT || !a->walls[cell_type])
        return a->walls[CELL_CONCRETE];   // repli sûr : béton
    return a->walls[cell_type];
}

const sfImage *assets_floor(const assets_t *a)
{
    return a->floor;
}

const sfImage *assets_ceiling(const assets_t *a)
{
    return a->ceiling;
}
