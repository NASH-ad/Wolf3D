//
// Raycaster — DDA ray casting (pure computation, no drawing)
//

#include <math.h>
#include "raycaster_internal.h"

// Lance un rayon pour une colonne d'écran.
// camera_x ∈ [-1, 1] : -1 = bord gauche de l'écran, 0 = centre, +1 = bord droit.
ray_t cast_ray(const level_t *lvl, const player_t *p, float camera_x)
{
    ray_t ray;
    // Direction du rayon = direction du joueur + décalage sur le plan caméra
    float ray_dir_x = p->dir.x + p->plane.x * camera_x;
    float ray_dir_y = p->dir.y + p->plane.y * camera_x;
    // Cellule courante
    int map_x = (int)p->pos.x;
    int map_y = (int)p->pos.y;
    // Distance que le rayon parcourt pour traverser une cellule entière
    float delta_x = (ray_dir_x == 0) ? 1e30f : fabsf(1.0f / ray_dir_x);
    float delta_y = (ray_dir_y == 0) ? 1e30f : fabsf(1.0f / ray_dir_y);
    // Sens de progression (+1 / -1) et distance jusqu'à la 1re frontière
    int step_x;
    int step_y;
    float side_x;   // distance cumulée jusqu'à la prochaine frontière verticale
    float side_y;

    if (ray_dir_x < 0) {
        step_x = -1;
        side_x = (p->pos.x - map_x) * delta_x;
    } else {
        step_x = 1;
        side_x = (map_x + 1.0f - p->pos.x) * delta_x;
    }
    if (ray_dir_y < 0) {
        step_y = -1;
        side_y = (p->pos.y - map_y) * delta_y;
    } else {
        step_y = 1;
        side_y = (map_y + 1.0f - p->pos.y) * delta_y;
    }
    // DDA : on avance de frontière en frontière jusqu'à toucher un mur
    while (1) {
        if (side_x < side_y) {
            side_x += delta_x;
            map_x += step_x;
            ray.side = 0;
        } else {
            side_y += delta_y;
            map_y += step_y;
            ray.side = 1;
        }
        if (level_is_wall(lvl, map_x, map_y))
            break;
    }
    ray.map_x = map_x;
    ray.map_y = map_y;
    // Distance perpendiculaire (et non euclidienne) : corrige l'effet fisheye
    if (ray.side == 0)
        ray.perp_dist = (side_x - delta_x);
    else
        ray.perp_dist = (side_y - delta_y);
    // Point d'impact exact sur le mur, pour la coordonnée de texture (usage futur)
    if (ray.side == 0)
        ray.wall_x = p->pos.y + ray.perp_dist * ray_dir_y;
    else
        ray.wall_x = p->pos.x + ray.perp_dist * ray_dir_x;
    ray.wall_x -= floorf(ray.wall_x);
    return ray;
}
