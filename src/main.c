//
// Wolf3D — entry point
//

#include "app.h"

#define WIN_W  1280
#define WIN_H  720

int main(void)
{
    app_t *app = app_create(WIN_W, WIN_H, "Wolf3D");

    if (!app)
        return 84;
    app_run(app);
    app_destroy(app);
    return 0;
}
