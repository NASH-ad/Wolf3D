//
// Renderer module — framebuffer implementation
//

#include <stdlib.h>
#include <math.h>
#include "raycaster_internal.h"

// Head-bobbing
#define BOB_AMPLITUDE  7.0f    // amplitude verticale en pixels (subtil)
#define BOB_SPEED      9.0f    // vitesse d'oscillation
#define MOVE_EPSILON   0.0001f // seuil pour considérer que Ralf a bougé

// Floor casting : nombre de lignes d'écran traitées par pas (1 = pleine réso,
// 2 = moitié verticale dupliquée → deux fois moins de calculs).
#define FLOOR_STEP     2

// Met à jour la phase de bobbing selon que Ralf a bougé, et renvoie
// l'horizon (ligne médiane) à utiliser pour toute la frame.
static int compute_horizon(renderer_t *r, const player_t *p)
{
    float dx = p->pos.x - r->prev_pos.x;
    float dy = p->pos.y - r->prev_pos.y;
    int moved = (dx * dx + dy * dy) > MOVE_EPSILON;
    float bob = 0.0f;

    r->prev_pos = p->pos;
    if (!r->bob_enabled)
        return r->height / 2;
    if (moved)
        r->bob_phase += BOB_SPEED * 0.016f;   // pas ~ constant par frame
    bob = sinf(r->bob_phase) * BOB_AMPLITUDE;
    return r->height / 2 + (int)bob;
}

renderer_t *renderer_create(sfRenderWindow *window, int width, int height,
                const assets_t *assets)
{
    renderer_t *r = malloc(sizeof(renderer_t));

    if (!r)
        return NULL;
    r->window = window;
    r->width = width;
    r->height = height;
    r->assets = assets;
    r->bob_phase = 0.0f;
    r->bob_enabled = 1;
    r->prev_pos = (vec2_t){0.0f, 0.0f};
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

void renderer_set_bob(renderer_t *r, int enabled)
{
    r->bob_enabled = enabled;
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

// Copie un pixel du framebuffer (src) vers un autre (dst) — pour dupliquer une ligne
static void copy_pixel(renderer_t *r, int dx, int dy, int sx, int sy)
{
    int dst = (dy * r->width + dx) * 4;
    int src = (sy * r->width + sx) * 4;

    r->pixels[dst + 0] = r->pixels[src + 0];
    r->pixels[dst + 1] = r->pixels[src + 1];
    r->pixels[dst + 2] = r->pixels[src + 2];
    r->pixels[dst + 3] = r->pixels[src + 3];
}

// Écrit une rangée de sol ET la rangée de plafond symétrique en un seul passage.
// y = ligne d'écran sous l'horizon ; sa symétrique (2*horizon - y) est le plafond.
static void cast_floor_row(renderer_t *r, const player_t *p, int y, int horizon)
{
    const sfImage *floor_img = assets_floor(r->assets);
    const sfImage *ceil_img = assets_ceiling(r->assets);
    // Distance de la rangée : constante pour toute la ligne
    int denom = y - horizon;
    float row_dist = (0.5f * r->height) / (denom == 0 ? 1 : denom);
    // Point du sol au bord gauche (dir - plane) et pas horizontal
    float fx = p->pos.x + row_dist * (p->dir.x - p->plane.x);
    float fy = p->pos.y + row_dist * (p->dir.y - p->plane.y);
    float step_x = row_dist * 2.0f * p->plane.x / r->width;
    float step_y = row_dist * 2.0f * p->plane.y / r->width;
    int ceil_y = 2 * horizon - y;

    for (int x = 0; x < r->width; x++) {
        int tx = (int)(WALL_TEX_SIZE * (fx - floorf(fx))) & (WALL_TEX_SIZE - 1);
        int ty = (int)(WALL_TEX_SIZE * (fy - floorf(fy))) & (WALL_TEX_SIZE - 1);
        if (y >= 0 && y < r->height)
            put_pixel(r, x, y, sfImage_getPixel(floor_img, tx, ty));
        if (ceil_y >= 0 && ceil_y < r->height)
            put_pixel(r, x, ceil_y, sfImage_getPixel(ceil_img, tx, ty));
        fx += step_x;
        fy += step_y;
    }
}

// Duplique une rangée déjà calculée sur les FLOOR_STEP-1 lignes suivantes
// (sol et plafond), pour éviter de recalculer la projection.
static void duplicate_rows(renderer_t *r, int y, int horizon)
{
    for (int k = 1; k < FLOOR_STEP; k++) {
        int sy = y + k;
        int cy = 2 * horizon - sy;
        if (sy >= 0 && sy < r->height)
            for (int x = 0; x < r->width; x++)
                copy_pixel(r, x, sy, x, y);
        if (cy >= 0 && cy < r->height)
            for (int x = 0; x < r->width; x++)
                copy_pixel(r, x, cy, x, 2 * horizon - y);
    }
}

static void cast_floor_ceiling(renderer_t *r, const player_t *p, int horizon)
{
    for (int y = horizon + 1; y < r->height; y += FLOOR_STEP) {
        cast_floor_row(r, p, y, horizon);
        if (FLOOR_STEP > 1)
            duplicate_rows(r, y, horizon);
    }
}

// Assombrit une couleur (faces N/S) pour un effet de relief
static sfColor shade(sfColor c, int side)
{
    if (side == 1) {
        c.r = c.r * 7 / 10;
        c.g = c.g * 7 / 10;
        c.b = c.b * 7 / 10;
    }
    return c;
}

// Colonne de texture (0..63) correspondant au point d'impact wall_x
static int tex_column(const ray_t *ray)
{
    int tx = (int)(ray->wall_x * WALL_TEX_SIZE);

    // Miroir selon la face touchée pour que la texture ne soit pas inversée
    if (ray->side == 0 && ray->wall_x > 0.5f)
        tx = WALL_TEX_SIZE - tx - 1;
    if (ray->side == 1 && ray->wall_x < 0.5f)
        tx = WALL_TEX_SIZE - tx - 1;
    if (tx < 0)
        tx = 0;
    if (tx >= WALL_TEX_SIZE)
        tx = WALL_TEX_SIZE - 1;
    return tx;
}

// Dessine la tranche verticale de mur texturée pour la colonne x
static void draw_wall_column(renderer_t *r, int x, const ray_t *ray,
                const level_t *lvl, int horizon)
{
    int line_height = (ray->perp_dist > 0.0001f)
        ? (int)(r->height / ray->perp_dist) : r->height;
    int draw_start = -line_height / 2 + horizon;
    int draw_end = line_height / 2 + horizon;
    int cell = level_tile(lvl, ray->map_x, ray->map_y);
    const sfImage *img = assets_wall(r->assets, cell);
    int tx = tex_column(ray);

    for (int y = draw_start; y < draw_end; y++) {
        if (y < 0 || y >= r->height)
            continue;
        // Coordonnée verticale dans la texture (0..63), centrée sur l'horizon
        int d = (y - horizon) * 256 + line_height * 128;
        int ty = ((d * WALL_TEX_SIZE) / line_height) / 256;
        if (ty < 0)
            ty = 0;
        if (ty >= WALL_TEX_SIZE)
            ty = WALL_TEX_SIZE - 1;
        sfColor c = sfImage_getPixel(img, tx, ty);
        put_pixel(r, x, y, shade(c, ray->side));
    }
}

void renderer_draw_frame(renderer_t *r, const level_t *lvl, const player_t *p)
{
    ray_t ray;
    float camera_x;
    int horizon = compute_horizon(r, p);

    cast_floor_ceiling(r, p, horizon);
    for (int x = 0; x < r->width; x++) {
        camera_x = 2.0f * x / (float)r->width - 1.0f;
        ray = cast_ray(lvl, p, camera_x);
        draw_wall_column(r, x, &ray, lvl, horizon);
    }
    sfTexture_updateFromPixels(r->screen_tex, r->pixels,
        r->width, r->height, 0, 0);
    sfRenderWindow_drawSprite(r->window, r->screen_sprite, NULL);
}
