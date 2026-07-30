//
// platform source file
//

#include "../../include/platform.h"
#include "../../include/wolf3d.h"

// -----------------------------------------------------------------------------------
// Life cycle
// ----------------------------------------------------------------------------------

platform_t *platform_create(int width, int height, const char *title)
{
    platform_t *p = malloc(sizeof(platform_t));

    if (!p) {
        log_error("Failed to allocate memory for platform_t struct");
        return NULL;
    }
    p->mode = (sfVideoMode){.width = width, .height = height, .bitsPerPixel = 32};
    p->window = sfRenderWindow_create(p->mode, title, sfClose, NULL);
    if (!p->window) {
        platform_destroy(p);
        log_error("Failed to create window");
        return NULL;
    }
    sfRenderWindow_setFramerateLimit(p->window, 60);
    p->clock = sfClock_create();
    if (!p->clock) {
        log_error("Failed to create clock");
        platform_destroy(p);
        return NULL;
    }
    return p;
}

void platform_destroy(platform_t *p)
{
    if (!p)
        return;
    if (p->window)
        sfRenderWindow_destroy(p->window);
    if (p->clock)
        sfClock_destroy(p->clock);
    free(p);
}

// -----------------------------------------------------------------------------
// Utilitaries
// -----------------------------------------------------------------------------

int platform_is_open(const platform_t *p)
{
    return sfRenderWindow_isOpen(p->window);
}

int platform_poll_event(platform_t *p, sfEvent *evt)
{
    return sfRenderWindow_pollEvent(p->window, evt);
}

int platform_begin_frame(platform_t *p)
{
    sfRenderWindow_clear(p->window, sfBlack);
    p->delta_time = sfTime_asSeconds(sfClock_restart(p->clock));
    return 0;
}

int platform_end_frame(platform_t *p)
{
    sfRenderWindow_display(p->window);
    return 0;
}

int platform_key_down(sfKeyCode key)
{
    return sfKeyboard_isKeyPressed(key);
}

int platform_mouse_down(sfMouseButton button)
{
    return sfMouse_isButtonPressed(button);
}

vec2_t platform_mouse_pos(const platform_t *p)
{
    sfVector2i pos = sfMouse_getPositionRenderWindow(p->window);
    
    return (vec2_t){.x = pos.x, .y = pos.y};
}
