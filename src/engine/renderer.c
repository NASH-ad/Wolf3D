//
// Renderer module — framebuffer implementation
//

#include <stdlib.h>
#include "raycaster_internal.h"

// Couleurs de fond (placeholder avant le vrai sol/plafond casté)
#define CEIL_R  30
#define CEIL_G  30
#define CEIL_B  40
#define FLOOR_R 45
#define FLOOR_G 35
#define FLOOR_B 30

renderer_t *renderer_create(sfRenderWindow *window, int width, int height)
{
    renderer_t *r = malloc(sizeof(renderer_t));

    if (!r)
        return NULL;
    r->window = window;
    r->width = width;
    r->height = height;
    r->pixels = malloc(sizeof(sfUint8) * width * height * 4);
    r->screen_tex = sfTexture_create(width, height);
    r->screen_sprite = sfSprite_create();
    if (!r->pixels || !r->screen_tex || !r->screen_sprite) {
        renderer_destroy(r);
        return NULL;
    }
    sfSprite_setTexture(r->screen_sprite, r->screen_tex, sfTrue);
    return r;
}

void renderer_destroy(renderer_t *r)
{
    if (!r)
        return;
    if (r->screen_sprite)
        sfSprite_destroy(r->screen_sprite);
    if (r->screen_tex)
        sfTexture_destroy(r->screen_tex);
    free(r->pixels);
    // r->window n'est PAS detruit ici : il appartient a platform_t
    free(r);
}

// Ecrit un pixel RGBA dans le framebuffer
static void put_pixel(renderer_t *r, int x, int y, sfColor c)
{
    int idx = (y * r->width + x) * 4;

    r->pixels[idx + 0] = c.r;
    r->pixels[idx + 1] = c.g;
    r->pixels[idx + 2] = c.b;
    r->pixels[idx + 3] = c.a;
}

// Remplit plafond (moitie haute) et sol (moitie basse) en couleur unie
static void clear_background(renderer_t *r)
{
    sfColor ceil = {CEIL_R, CEIL_G, CEIL_B, 255};
    sfColor floor = {FLOOR_R, FLOOR_G, FLOOR_B, 255};
    int half = r->height / 2;

    for (int y = 0; y < r->height; y++)
        for (int x = 0; x < r->width; x++)
            put_pixel(r, x, y, (y < half) ? ceil : floor);
}

// Couleur du mur (placeholder — sera remplace par l'echantillonnage de texture)
static sfColor wall_color(const ray_t *ray)
{
    sfColor base;

    if ((ray->map_x + ray->map_y) % 2 == 0)
        base = (sfColor){170, 60, 60, 255};
    else
        base = (sfColor){70, 90, 130, 255};
    if (ray->side == 1) {
        base.r = base.r * 7 / 10;
        base.g = base.g * 7 / 10;
        base.b = base.b * 7 / 10;
    }
    return base;
}

// Dessine la tranche verticale de mur pour la colonne x
static void draw_wall_column(renderer_t *r, int x, const ray_t *ray)
{
    int line_height = (ray->perp_dist > 0.0001f)
        ? (int)(r->height / ray->perp_dist) : r->height;
    int draw_start = -line_height / 2 + r->height / 2;
    int draw_end = line_height / 2 + r->height / 2;
    sfColor col = wall_color(ray);

    if (draw_start < 0)
        draw_start = 0;
    if (draw_end >= r->height)
        draw_end = r->height - 1;
    for (int y = draw_start; y <= draw_end; y++)
        put_pixel(r, x, y, col);
}

void renderer_draw_frame(renderer_t *r, const level_t *lvl, const player_t *p)
{
    ray_t ray;
    float camera_x;

    clear_background(r);
    for (int x = 0; x < r->width; x++) {
        camera_x = 2.0f * x / (float)r->width - 1.0f;
        ray = cast_ray(lvl, p, camera_x);
        draw_wall_column(r, x, &ray);
    }
    sfTexture_updateFromPixels(r->screen_tex, r->pixels,
        r->width, r->height, 0, 0);
    sfRenderWindow_drawSprite(r->window, r->screen_sprite, NULL);
}
