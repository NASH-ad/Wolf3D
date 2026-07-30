//
// Session module — state of a single playthrough (opaque type)
//

#ifndef SESSION_H
#define SESSION_H

#include "wolf3d.h"
#include "level.h"
#include "player.h"

// --- Life cycle ---
session_t *session_create(void);
void       session_destroy(session_t *s);

// --- Access to the session state (player_t is mutable, not level_t) ---
player_t      *session_player(session_t *s);
const level_t *session_level(const session_t *s);
int            session_floor(const session_t *s);
int            session_score(const session_t *s);

// --- Progression ---
void session_add_score(session_t *s, int points);
void session_next_floor(session_t *s);   // free the current stage and generate the next one 

#endif // SESSION_H
