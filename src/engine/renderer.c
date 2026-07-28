//
// Renderer module — draws the scene using cast_ray results
//

#include <stdlib.h>
#include "raycaster_internal.h"

renderer_t *renderer_create(sfRenderWindow *window, int width, int height)
{
    renderer_t *r = malloc(sizeof(renderer_t));

    if (!r) {
        log_error("Failed to allocate memory for renderer_t");
        return NULL;
    }
    r->window = window;
    r->width = width;
    r->height = height;
    r->column = sfRectangleShape_create();
    if (!r->column) {
        free(r);
        log_error("Failed to allocate memory for col rectangle");
        return NULL;
    }
    return r;
}

void renderer_destroy(renderer_t *r)
{
    if (!r)
        return;
    if (r->column)
        sfRectangleShape_destroy(r->column);
    free(r);
}

static sfColor wall_color(const ray_t *ray)
{
    sfColor base;

    // Teinte selon la parité de la cellule — placeholder avant les vraies textures
    if ((ray->map_x + ray->map_y) % 2 == 0)
        base = (sfColor){170, 60, 60, 255};    // rouge sourd
    else
        base = (sfColor){70, 90, 130, 255};    // bleu-gris
    // Faces N/S assombries → effet de relief gratuit
    if (ray->side == 1) {
        base.r = base.r * 7 / 10;
        base.g = base.g * 7 / 10;
        base.b = base.b * 7 / 10;
    }
    return base;
}

// Draw the wall column hitted by the ray
static void draw_wall_column(renderer_t *r, int x, const ray_t *ray)
{
    int line_height = (ray->perp_dist > 0)
        ? (int)(r->height / ray->perp_dist) : r->height;
    int draw_start = -line_height / 2 + r->height / 2;

    if (draw_start < 0)
        draw_start = 0;
    sfRectangleShape_setPosition(r->column, (sfVector2f){(float)x, (float)draw_start});
    sfRectangleShape_setSize(r->column, (sfVector2f){1.0f, (float)line_height});
    sfRectangleShape_setFillColor(r->column, wall_color(ray));
    sfRenderWindow_drawRectangleShape(r->window, r->column, NULL);
}

void renderer_draw_frame(renderer_t *r, const level_t *lvl, const player_t *p)
{
    ray_t ray;
    float camera_x;

    for (int x = 0; x < r->width; x++) {
        camera_x = 2.0f * x / (float)r->width - 1.0f;
        ray = cast_ray(lvl, p, camera_x);
        draw_wall_column(r, x, &ray);
    }
}
