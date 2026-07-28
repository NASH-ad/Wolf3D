///
// Renderer module — raycasting engine (reads player + level, draws)
//

#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.h>
#include "wolf3d.h"
#include "player.h"
#include "level.h"

typedef struct renderer {
    sfRenderWindow *window;      // a simple reference to the window on which to draw
    int             width;
    int             height;
    sfRectangleShape *column;    // used to render each ray of the raycasting
} renderer_t;

// --- Life cycle ---
renderer_t *renderer_create(sfRenderWindow *window, int width, int height);
void        renderer_destroy(renderer_t *r);

// --- Rendering ---
void        renderer_draw_frame(renderer_t *r, const level_t *lvl, const player_t *p);

#endif // RENDERER_H