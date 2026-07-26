//
// platform module
// API between the graphic lib and the system
//

#ifndef PLATFORM_H
#define PLATFORM_H

    #include "wolf3d.h"
    #include <SFML/Audio.h>
    #include <SFML/Graphics.h>
    #include <SFML/System.h>

typedef struct platform {
    sfRenderWindow *window;
    sfVideoMode mode;
    sfClock *clock;
    float delta_time; // seconds since the  last frame
} platform_t;

// Life cycle
platform_t *platform_create(int width, int height, const char *title);
void        platform_destroy(platform_t *p);

// Loop
int         platform_is_open(const platform_t *p);
int         platform_poll_event(platform_t *p, sfEvent *evt);
int         platform_begin_frame(platform_t *p);  // clear the screen and update delta_time
int         platform_end_frame(platform_t *p); // display the frame

// Input 
int         platform_key_dowm(sfKeyCode key);
int         platform_mouse_down(sfMouseButton button);
vec2_t      platform_mouse_pos(const platform_t *p);

#endif