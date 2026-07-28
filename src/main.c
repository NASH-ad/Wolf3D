//
// Wolf3D — test main: assemble platform + level + player + renderer
//

#include <SFML/Graphics.h>
#include "platform.h"
#include "level.h"
#include "player.h"
#include "renderer.h"
#include "wolf3d.h"

#define WIN_W  1280
#define WIN_H  720

// Regroupe les objets du jeu pour rester sous la limite de paramètres
typedef struct game {
    platform_t *platform;
    renderer_t *renderer;
    level_t    *level;
    player_t    player;
} game_t;

// --- Événements fenêtre (fermeture) ---
static void handle_events(game_t *g)
{
    sfEvent event;

    while (platform_poll_event(g->platform, &event)) {
        if (event.type == sfEvtClosed)
            sfRenderWindow_close(g->platform->window);
        if (event.type == sfEvtKeyPressed
            && event.key.code == sfKeyEscape)
            sfRenderWindow_close(g->platform->window);
    }
}

// --- Entrées clavier (état instantané) ---
// NB clavier AZERTY : SFML lit la POSITION physique nommée en QWERTY.
// Les touches physiques Z/Q/S/D d'un AZERTY correspondent donc à W/A/S/D ici.
static void handle_input(game_t *g, float dt)
{
    player_t *p = &g->player;

    if (platform_key_down(sfKeyW) || platform_key_down(sfKeyUp))
        player_move(p, g->level, 1.0f, dt);
    if (platform_key_down(sfKeyS) || platform_key_down(sfKeyDown))
        player_move(p, g->level, -1.0f, dt);
    if (platform_key_down(sfKeyA))
        player_strafe(p, g->level, -1.0f, dt);
    if (platform_key_down(sfKeyD))
        player_strafe(p, g->level, 1.0f, dt);
    if (platform_key_down(sfKeyLeft))
        player_rotate(p, 1.0f, dt);
    if (platform_key_down(sfKeyRight))
        player_rotate(p, -1.0f, dt);
}

// --- Boucle principale ---
static void game_loop(game_t *g)
{
    while (platform_is_open(g->platform)) {
        handle_events(g);
        handle_input(g, g->platform->delta_time);
        platform_begin_frame(g->platform);
        renderer_draw_frame(g->renderer, g->level, &g->player);
        platform_end_frame(g->platform);
    }
}

// --- Mise en place / libération ---
static int game_setup(game_t *g)
{
    g->platform = platform_create(WIN_W, WIN_H, "Wolf3D");
    if (!g->platform)
        return 0;
    g->level = level_create(1);
    if (!g->level)
        return 0;
    g->renderer = renderer_create(g->platform->window, WIN_W, WIN_H);
    if (!g->renderer)
        return 0;
    player_init(&g->player, level_spawn(g->level));
    return 1;
}

static void game_teardown(game_t *g)
{
    if (g->renderer)
        renderer_destroy(g->renderer);
    if (g->level)
        level_destroy(g->level);
    if (g->platform)
        platform_destroy(g->platform);
}

int main(void)
{
    game_t g = {0};

    if (!game_setup(&g)) {
        game_teardown(&g);
        return 84;
    }
    game_loop(&g);
    game_teardown(&g);
    return 0;
}