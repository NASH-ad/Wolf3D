//
// Player module — Ralf's state and movement (implementation)
//

#include "../../include/player.h"
#include "../../include/wolf3d.h"
 
#define FOV_PLANE   0.66f    // longueur du plan caméra → ~66° de champ de vision
#define START_HP    100
#define MOVE_SPEED  3.0f     // cellules par seconde
#define ROT_SPEED   2.5f     // radians par seconde
#define WALL_MARGIN 0.15f    // distance mini conservée entre Ralf et un mur
 
void player_init(player_t *p, vec2_t spawn)
{
    // Centre de la cellule de spawn (pos exprimée en unités de grille)
    p->pos.x = spawn.x + 0.5f;
    p->pos.y = spawn.y + 0.5f;
    // Direction initiale + plan caméra perpendiculaire
    p->dir = (vec2_t){-1.0f, 0.0f};
    p->plane = (vec2_t){0.0f, FOV_PLANE};
    p->move_speed = MOVE_SPEED;
    p->rot_speed = ROT_SPEED;
    p->hp = START_HP;
}
 
// Tente de déplacer Ralf vers (nx, ny) en glissant le long des murs :
// chaque axe est testé séparément, donc un blocage sur X n'empêche pas Y.
static void try_move(player_t *p, const level_t *lvl, float nx, float ny)
{
    // Axe X : on teste la cellule visée en gardant une marge anti-clipping
    float probe_x = nx + (nx > p->pos.x ? WALL_MARGIN : -WALL_MARGIN);
    if (!level_is_wall(lvl, (int)probe_x, (int)p->pos.y))
        p->pos.x = nx;
    // Axe Y : idem, indépendamment de ce qui s'est passé sur X
    float probe_y = ny + (ny > p->pos.y ? WALL_MARGIN : -WALL_MARGIN);
    if (!level_is_wall(lvl, (int)p->pos.x, (int)probe_y))
        p->pos.y = ny;
}
 
// forward > 0 : avance ; forward < 0 : recule
void player_move(player_t *p, const level_t *lvl, float forward, float delta)
{
    float step = forward * p->move_speed * delta;
    float nx = p->pos.x + p->dir.x * step;
    float ny = p->pos.y + p->dir.y * step;
 
    try_move(p, lvl, nx, ny);
}
 
// side > 0 : pas vers la droite ; side < 0 : pas vers la gauche
// On se déplace le long du plan caméra (perpendiculaire à la direction).
void player_strafe(player_t *p, const level_t *lvl, float side, float delta)
{
    float step = side * p->move_speed * delta;
    float nx = p->pos.x + p->plane.x * step;
    float ny = p->pos.y + p->plane.y * step;
 
    try_move(p, lvl, nx, ny);
}
 
// dir > 0 : rotation dans un sens ; dir < 0 : dans l'autre.
// dir ET plane doivent tourner ensemble pour garder le plan perpendiculaire.
void player_rotate(player_t *p, float sign, float delta)
{
    float angle = (sign / sign) * p->rot_speed * delta;
    float c = cosf(angle);
    float s = sinf(angle);
    float old_dx = p->dir.x;
    float old_px = p->plane.x;
 
    p->dir.x = p->dir.x * c - p->dir.y * s;
    p->dir.y = old_dx * s + p->dir.y * c;
    p->plane.x = p->plane.x * c - p->plane.y * s;
    p->plane.y = old_px * s + p->plane.y * c;
}
