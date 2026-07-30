//
// Renderer module — raycasting engine (framebuffer-based)
//

#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.h>
#include "wolf3d.h"
#include "player.h"
#include "level.h"
#include "assets.h"

typedef struct renderer {
    sfRenderWindow *window;         // cible de dessin (empruntée à platform_t)
    int             width;
    int             height;
    sfUint8        *pixels;         // framebuffer RGBA : width * height * 4
    sfTexture      *screen_tex;     // texture GPU, mise à jour chaque frame
    sfSprite       *screen_sprite;  // sprite plein écran qui affiche screen_tex
    const assets_t *assets;         // textures de murs (empruntées, non possédées)
} renderer_t;

// --- Cycle de vie ---
renderer_t *renderer_create(sfRenderWindow *window, int width, int height,
                const assets_t *assets);
void        renderer_destroy(renderer_t *r);

// --- Rendu d'une frame de jeu (lit le joueur et le niveau en const) ---
void        renderer_draw_frame(renderer_t *r, const level_t *lvl, const player_t *p);

#endif // RENDERER_H