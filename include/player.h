//
// Player module — state and movement (no graphics dependency)
//

#ifndef PLAYER_H
#define PLAYER_H

#include "wolf3d.h"
#include "level.h"

typedef struct player {
    vec2_t  pos;         // position sur la grille (unités = cellules)
    vec2_t  dir;         // vecteur direction (normalisé)
    vec2_t  plane;       // plan caméra, perpendiculaire à dir → contrôle le FOV
    float   move_speed;  // cellules par seconde
    float   rot_speed;   // radians par seconde
    int     hp;
} player_t;

// --- Life cycle ---
void player_init(player_t *p, vec2_t spawn);

// --- Movement (delta = temps écoulé depuis la dernière frame) ---
void player_move(player_t *p, const level_t *lvl, float forward, float delta);
void player_strafe(player_t *p, const level_t *lvl, float side, float delta);
void player_rotate(player_t *p, float dir, float delta);

#endif // PLAYER_H