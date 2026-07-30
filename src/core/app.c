//
// App module — implementation
//

#include <stdlib.h>
#include <SFML/Graphics.h>
#include "app.h"
#include "platform.h"
#include "renderer.h"
#include "session.h"
#include "assets.h"

// Définition interne : invisible du reste du projet (type opaque).
// Version réduite : on ajoutera audio / ui / settings au fil des modules.
struct app {
    platform_t *platform;
    assets_t   *assets;
    renderer_t *renderer;
    session_t  *session;
};

// --- Événements fenêtre (fermeture, Échap) ---
static void app_handle_events(app_t *app)
{
    sfEvent event;

    while (platform_poll_event(app->platform, &event)) {
        if (event.type == sfEvtClosed)
            sfRenderWindow_close(app->platform->window);
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape)
            sfRenderWindow_close(app->platform->window);
    }
}

// --- Entrées clavier (état instantané) ---
// NB AZERTY : SFML lit la POSITION physique nommée en QWERTY. Les touches
// physiques Z/Q/S/D d'un clavier AZERTY déclenchent donc W/A/S/D ici.
static void app_handle_input(app_t *app, float dt)
{
    player_t      *p = session_player(app->session);
    const level_t *lvl = session_level(app->session);

    if (platform_key_down(sfKeyW) || platform_key_down(sfKeyUp))
        player_move(p, lvl, 1.0f, dt);
    if (platform_key_down(sfKeyS) || platform_key_down(sfKeyDown))
        player_move(p, lvl, -1.0f, dt);
    if (platform_key_down(sfKeyA))
        player_strafe(p, lvl, -1.0f, dt);
    if (platform_key_down(sfKeyD))
        player_strafe(p, lvl, 1.0f, dt);
    if (platform_key_down(sfKeyLeft))
        player_rotate(p, 1.0f, dt);
    if (platform_key_down(sfKeyRight))
        player_rotate(p, -1.0f, dt);
}

app_t *app_create(int width, int height, const char *title)
{
    app_t *app = malloc(sizeof(struct app));

    if (!app)
        return NULL;
    app->platform = NULL;
    app->assets = NULL;
    app->renderer = NULL;
    app->session = NULL;
    app->platform = platform_create(width, height, title);
    if (!app->platform)
        return (app_destroy(app), NULL);
    app->assets = assets_create();
    if (!app->assets)
        return (app_destroy(app), NULL);
    app->renderer = renderer_create(app->platform->window, width, height,
        app->assets);
    if (!app->renderer)
        return (app_destroy(app), NULL);
    app->session = session_create();
    if (!app->session)
        return (app_destroy(app), NULL);
    return app;
}

void app_destroy(app_t *app)
{
    if (!app)
        return;
    if (app->session)
        session_destroy(app->session);
    if (app->renderer)
        renderer_destroy(app->renderer);
    if (app->assets)
        assets_destroy(app->assets);
    if (app->platform)
        platform_destroy(app->platform);
    free(app);
}

void app_run(app_t *app)
{
    while (platform_is_open(app->platform)) {
        app_handle_events(app);
        app_handle_input(app, app->platform->delta_time);
        platform_begin_frame(app->platform);
        renderer_draw_frame(app->renderer,
            session_level(app->session), session_player(app->session));
        platform_end_frame(app->platform);
    }
}
