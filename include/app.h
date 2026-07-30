//
// App module — top-level lifecycle and game loop (opaque type)
//

#ifndef APP_H
#define APP_H

#include "wolf3d.h"

// --- Cycle de vie ---
app_t *app_create(int width, int height, const char *title);
void   app_destroy(app_t *app);

// --- Boucle principale (bloque jusqu'à fermeture de la fenêtre) ---
void   app_run(app_t *app);

#endif // APP_H
