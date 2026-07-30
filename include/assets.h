//
// Assets module — loads wall textures once, indexed by cell type
//

#ifndef ASSETS_H
#define ASSETS_H

#include <SFML/Graphics.h>
#include "wolf3d.h"
#include "level.h"

#define WALL_TEX_COUNT  6      // index 0 (sol) inutilisé + 5 types de murs
#define WALL_TEX_SIZE   64     // toutes les textures sont en 64x64

typedef struct assets {
    // Indexé par cell_t : walls[CELL_CINDER], walls[CELL_STEEL], etc.
    // walls[CELL_FLOOR] (index 0) reste NULL — le sol n'est pas un mur.
    sfImage *walls[WALL_TEX_COUNT];
} assets_t;

// --- Cycle de vie ---
assets_t *assets_create(void);      // charge tous les .png ; NULL si échec
void      assets_destroy(assets_t *a);

// --- Accès ---
// Renvoie l'image de mur pour un type de cellule (jamais NULL pour un mur valide).
const sfImage *assets_wall(const assets_t *a, int cell_type);

#endif // ASSETS_H
