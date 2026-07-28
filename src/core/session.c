//
// Session module — implementation
//

#include <stdlib.h>
#include "session.h"

// Internal definition (type opaque)
struct session {
    level_t *level;
    player_t player;
    int      floor_number;
    int      score;
};

session_t *session_create(void)
{
    session_t *s = malloc(sizeof(struct session));

    if (!s) {
        log_error("Failed to allocate memory for session_t");
        return NULL;
    }
    s->floor_number = 1;
    s->score = 0;
    s->level = level_create(s->floor_number);
    if (!s->level) {
        free(s);
        return NULL;
    }
    player_init(&s->player, level_spawn(s->level));
    return s;
}

void session_destroy(session_t *s)
{
    if (!s)
        return;
    if (s->level)
        level_destroy(s->level);
    free(s);
}

player_t *session_player(session_t *s)
{
    return &s->player;
}

const level_t *session_level(const session_t *s)
{
    return s->level;
}

int session_floor(const session_t *s)
{
    return s->floor_number;
}

int session_score(const session_t *s)
{
    return s->score;
}

void session_add_score(session_t *s, int points)
{
    s->score += points;
}

void session_next_floor(session_t *s)
{
    level_destroy(s->level);
    s->floor_number++;
    s->level = level_create(s->floor_number);
    if (s->level)
        player_init(&s->player, level_spawn(s->level));
}
