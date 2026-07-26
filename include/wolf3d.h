//
// Shared structures header
//

#ifndef WOLF_H
    #define WOLF_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>

    // --- Error logging (rouge sur stderr) ---
    #define ERR_RED    "\033[1;31m"
    #define ERR_RESET  "\033[0m"

    #define log_error(fmt, ...) fprintf(stderr, ERR_RED "[ERROR] " fmt ERR_RESET "\n", ##__VA_ARGS__)

// --- Global constraints ---
    #define REF_WIDTH 1280
    #define REF_HEIGHT 720

// --- Game data types ---
typedef struct vec2 {
    float x;
    float y;
} vec2_t;

// --- forward declaration of data types from each module ---
typedef struct app app_t;
typedef struct session session_t;
typedef struct level level_t;
typedef struct player player_t;
typedef struct platform platform_t;
typedef struct assets assets_t;
typedef struct audio audio_t;
typedef struct ui ui_t;

// --- State machine enums ---
typedef enum {
    S_MAIN,
    S_GAME,
    S_PAUSE,
    S_SETTING_VOLUME,
    S_SETTING_WINDOW,
    S_SETTING_CONTROLLER,
    S_SCORES,
    S_GETNAME,
} interface_state_t;

#endif