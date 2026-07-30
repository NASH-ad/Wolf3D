//
// Engine internal header — shared between raycaster.c and renderer.c
//

#ifndef RAYCASTER_INTERNAL_H
#define RAYCASTER_INTERNAL_H

#include "../../include/renderer.h"
#include "../../include/player.h"
#include "../../include/level.h"

// Résultat d'un lancer de rayon pour une colonne d'écran.
// Contient déjà tout ce qu'il faudra pour les textures (wall_x, map coords),
// donc l'ajout des textures ne touchera pas cast_ray().
typedef struct ray {
    float perp_dist;   // distance perpendiculaire au mur (corrige le fisheye)
    int   side;        // 0 = mur touché sur une face E/O, 1 = face N/S
    int   map_x;       // cellule du mur touché
    int   map_y;
    float wall_x;      // position [0,1] de l'impact sur le mur → coord. de texture
} ray_t;

// raycaster.c
ray_t cast_ray(const level_t *lvl, const player_t *p, float camera_x);

#endif // RAYCASTER_INTERNAL_H