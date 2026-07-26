//
// Level module implementation
//

#include "../../include/level.h"
#include "../../include/wolf3d.h"
#include <time.h>

#define MAP_W       48
#define MAP_H       48
#define MIN_LEAF    12  // the min area to stop the recursive cut
#define MIN_ROOM    4   // minimal size of a room
#define MAX_ROOMS   64  // max number of rooms

struct level {
    cell_t **grid;
    int w;
    int h;
    vec2_t spawn;
    vec2_t exit;
};

typedef struct rect {
    int x;
    int y;
    int w;
    int h;
} rect_t;

typedef struct bsp_node {
    rect_t area;
    struct bsp_node *left;
    struct bsp_node *right;
} bsp_node_t;

typedef struct gen_ctx {
    level_t *lvl;
    vec2_t rooms[MAX_ROOMS]; // rooms' centers ordered by generation time
    int room_count;
} gen_ctx_t;

// --------------------------------------------------------------------------------
// ------------------------------ UTILITARIES -------------------------------------
// --------------------------------------------------------------------------------

static void seed_rng(void)
{
    static int seeded = 0;

    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }
}

static int rand_range(int min, int max)
{
    if (max <= min) {
        log_error("Bad params for rand_range(): max <= min");
        return min;
    }
    return min + (rand() % (max - min + 1));
}

// --------------------------------------------------------------------------------
// ------------------------- MEMORY ALLOCATOR -------------------------------------
// --------------------------------------------------------------------------------

static cell_t **grid_alloc(int w, int h)
{
    cell_t **grid = malloc(sizeof(cell_t *) * h);

    if (!grid) {
        log_error("Failed to allocate memory for grid");
        return NULL;
    }

    for (int y = 0; y < h; y++) {
        grid[y] = malloc(sizeof(cell_t) * w);
        
        if (!grid[y]) {
            log_error("Failed to allocate memory for grid");
            for (int i = 0; i < y; i++)
                free(grid[i]);
            free(grid);
            return NULL;
        }

        for (int i = 0; i < w; i++)
            grid[y][i] = CELL_WALL;
    }

    return grid;
}

// --------------------------------------------------------------------------------
// -------------------------- BSP TREE GENERATION ---------------------------------
// --------------------------------------------------------------------------------

static bsp_node_t *node_create(int x, int y, int w, int h)
{
    bsp_node_t *node = malloc(sizeof(bsp_node_t));

    if (!node) {
        log_error("Failed to allocate memory for bsp_node");
        return NULL;
    }
    node->area = (rect_t){.x = x, .y = y, .h = h, .w = w};
    node->left = NULL;
    node->right = NULL;
    return node;
}

static bsp_free(bsp_node_t *node)
{
    if (!node)
        return;
    bsp_free(node->left);
    bsp_free(node->right);
    free(node);
}

static int node_split(bsp_node_t *node)
{
    int horizontal = rand_range(0, 1);
    int extent;
    int cut;
    rect_t a = node->area;

    if (a.w > a.h && a.w / (float)a.h >= 1.25f)
        horizontal = 1;
    else if (a.h > a.w && a.h / (float)a.w >= 1.25f)
        horizontal = 0;
    extent = horizontal ? a.h : a.w;
    if (extent < MIN_LEAF * 2)
        return 0;
    cut = rand_range(MIN_LEAF, extent - MIN_LEAF);
    if (horizontal) {
        node->left = node_create(a.x, a.y, a.w, cut);
        node->right = node_create(a.x, a.y + cut, a.w, a.h - cut);
    } else {
        node->left = node_create(a.x, a.y, cut, a.h);
        node->right = node_create(a.x + cut, a.y, a.w - cut, a.h);
    }
    return (node->left && node->right);
}

static void bsp_build(bsp_node_t *node)
{
    if (!node || !node_split(node))
        return;
    bsp_build(node->left);
    bsp_build(node->right);
}

// --------------------------------------------------------------------------------
// ------------------------------- ROOM CARVING -----------------------------------
// --------------------------------------------------------------------------------

static void carve_room(gen_ctx_t *ctx, rect_t area)
{
    int rw = rand_range(MIN_ROOM, area.w - 2);
    int rh = rand_range(MIN_ROOM, area.h - 2);
    int rx = rand_range(area.x, area.x + rw - 1);
    int ry = rand_range(area.y, area.y + rh - 1);

    for (int m = ry; m < ry + rh; m++) {
        for (int n = rx; n < rx + rw; n++)
            ctx->lvl->grid[m][n] = CELL_FLOOR;
    }
    if (ctx->room_count < MAX_ROOMS) {
        ctx->rooms[ctx->room_count].x = rx + (rw / 2);
        ctx->rooms[ctx->room_count].y = ry + (rh / 2);
        ctx->room_count += 1;
    }
}

static void carve_leaves(gen_ctx_t *ctx, bsp_node_t *node)
{
    if (!node)
        return;
    if (!node->left && !node->right) {
        if (node->area.w > MIN_ROOM + 2 && node->area.h > MIN_ROOM + 2)
            carve_room(ctx, node->area);
        return;
    }
    carve_leaves(ctx, node->left);
    carve_leaves(ctx, node->right);
}

// --------------------------------------------------------------------------------
// ---------------------------- CORRIDOR CARVING ----------------------------------
// --------------------------------------------------------------------------------

static void carve_corridor(level_t *lvl, vec2_t a, vec2_t b)
{
    int x = (int)a.x;
    int y = (int)a.y;
    int tx = (int)b.x;
    int ty = (int)b.y;
 
    while (x != tx) {
        lvl->grid[y][x] = CELL_FLOOR;
        x += (tx > x) ? 1 : -1;
    }
    while (y != ty) {
        lvl->grid[y][x] = CELL_FLOOR;
        y += (ty > y) ? 1 : -1;
    }
    lvl->grid[y][x] = CELL_FLOOR;
}
 
static void connect_rooms(gen_ctx_t *ctx)
{
    for (int i = 1; i < ctx->room_count; i++)
        carve_corridor(ctx->lvl, ctx->rooms[i - 1], ctx->rooms[i]);
}

// --------------------------------------------------------------------------------
// ------------------------------ SPAWN AND EXIT ----------------------------------
// --------------------------------------------------------------------------------

static void place_spawn_exit(gen_ctx_t *ctx)
{
    float best = -1.0f;
    int   far = 0;
 
    if (ctx->room_count == 0) {
        ctx->lvl->spawn = (vec2_t){1.5f, 1.5f};
        ctx->lvl->exit = (vec2_t){1.5f, 1.5f};
        return;
    }
    ctx->lvl->spawn = ctx->rooms[0];
    for (int i = 1; i < ctx->room_count; i++) {
        float dx = ctx->rooms[i].x - ctx->rooms[0].x;
        float dy = ctx->rooms[i].y - ctx->rooms[0].y;
        float d = dx * dx + dy * dy;
        if (d > best) {
            best = d;
            far = i;
        }
    }
    ctx->lvl->exit = ctx->rooms[far];
}

// -------------------------------------------------------------------------
// -------------------------- API publique ---------------------------------
// -------------------------------------------------------------------------
 
level_t *level_create(int floor_number)
{
    level_t   *lvl = malloc(sizeof(struct level));
    bsp_node_t *root;
    gen_ctx_t  ctx;
 
    (void)floor_number;   // servira plus tard à moduler la difficulté
    seed_rng();
    if (!lvl) {
        log_error("Failed to allocate memory for level struct");
        return NULL;
    }
    lvl->w = MAP_W;
    lvl->h = MAP_H;
    lvl->grid = grid_alloc(MAP_W, MAP_H);
    if (!lvl->grid) {
        free(lvl);
        return NULL;
    }
    root = node_create(0, 0, MAP_W, MAP_H);
    if (!root) {
        level_destroy(lvl);
        log_error("Failed to allocate memory for bsp root node");
        return NULL;
    }
    ctx.lvl = lvl;
    ctx.room_count = 0;
    bsp_build(root);
    carve_leaves(&ctx, root);
    connect_rooms(&ctx);
    place_spawn_exit(&ctx);
    bsp_free(root);
    return lvl;
}
 
void level_destroy(level_t *lvl)
{
    if (!lvl)
        return;
    if (lvl->grid) {
        for (int y = 0; y < lvl->h; y++)
            free(lvl->grid[y]);
        free(lvl->grid);
    }
    free(lvl);
}
 
int level_width(const level_t *lvl)
{
    return lvl->w;
}
 
int level_height(const level_t *lvl)
{
    return lvl->h;
}
 
int level_is_wall(const level_t *lvl, int x, int y)
{
    if (x < 0 || y < 0 || x >= lvl->w || y >= lvl->h)
        return 1;   // hors carte = mur (Ralf ne peut pas sortir)
    return lvl->grid[y][x] == CELL_WALL;
}
 
vec2_t level_spawn(const level_t *lvl)
{
    return lvl->spawn;
}
 
vec2_t level_exit(const level_t *lvl)
{
    return lvl->exit;
}
 
void level_print_ascii(const level_t *lvl)
{
    for (int y = 0; y < lvl->h; y++) {
        for (int x = 0; x < lvl->w; x++) {
            if (x == (int)lvl->spawn.x && y == (int)lvl->spawn.y)
                putchar('S');
            else if (x == (int)lvl->exit.x && y == (int)lvl->exit.y)
                putchar('E');
            else if (lvl->grid[y][x] == CELL_WALL)
                putchar('#');
            else
                putchar('.');
        }
        putchar('\n');
    }
}
