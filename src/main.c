//
// main file
//

#include "../include/wolf3d.h"
#include "../include/platform.h"

int main(void)
{
    platform_t *platform = platform_create(1280, 720, "Wolf3D");
    if (!platform)
        return 1;
    sfEvent event = {0};

    while (platform_is_open(platform)) {
        while (platform_poll_event(platform, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(platform->window);
        }
        platform_begin_frame(platform);
        platform_end_frame(platform);
    }
    platform_destroy(platform);
    return 0;
}