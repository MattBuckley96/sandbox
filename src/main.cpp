#include <memory>

#include "config.hpp"
#include "world.hpp"
#include "player.hpp"

////////////////////////////////////////////////////////////////////////////////

Texture blockAtlas; 
Texture itemAtlas;
Texture playerTexture; 
Texture blockBreakTexture; 
Texture treeTexture;

////////////////////////////////////////////////////////////////////////////////

#define KiB(x) (x * 1024)
#define MiB(x) (KiB(x) * 1024)
#define GiB(x) (MiB(x) * 1024)

struct Arena
{
    size_t size;
    size_t capacity;
    char* memory;
};

////////////////////////////////////////////////////////////////////////////////

void arena_init(Arena& arena, size_t capacity)
{
    arena.size = 0;
    arena.capacity = capacity;

    arena.memory = (char*)malloc(capacity);
    if (arena.memory)
    {
        memset(arena.memory, 0, capacity);
    }
}

void arena_free(Arena& arena)
{
    free(arena.memory);
}

void* arena_alloc(Arena& arena, size_t size)
{
    void* ptr = nullptr;

    size = (size + 7) & ~7;
    if ((arena.size + size) <= arena.capacity)
    {
        ptr = arena.memory + arena.size;
        arena.size += size;
    }

    return ptr;
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sandbox");

    blockAtlas = LoadTexture("assets/block_atlas.png");
    itemAtlas = LoadTexture("assets/item_atlas.png");
    playerTexture = LoadTexture("assets/player.png");
    blockBreakTexture = LoadTexture("assets/block_break.png");
    treeTexture = LoadTexture("assets/tree.png");

    Arena arena = {};
    arena_init(arena, sizeof(World));

    Player player = {
        .accel = 9.0f,
        .friction = 10.0f,
        .maxSpeed = 300.0f,
        .gravity = 860.0f,
        .jumpForce = 465.0f,
        .jumpBuffer = 0.2f,
        .coyoteBuffer = 0.1f,
        .mineSpeed = 0.25f,
        .placeSpeed = 0.25f,
    };

    Camera2D camera = {
        .offset = screenCenter,
        .target = player.pos,
        .zoom = 1.0f,
    };

    World* world = (World*)arena_alloc(arena, sizeof(World));
    if (world == nullptr)
    {
        return -1;
    }

    world->camera = &camera;

    /*
        TODO: make this smaller somehow idk
        world is too big, texture no load
    */
    world->blockMap = LoadRenderTexture(
        WORLD_WIDTH * TILE_SIZE,
        WORLD_HEIGHT * TILE_SIZE
    );
    world_generate(*world);

    player.spawnPos = Vector2Scale(world->spawn, TILE_SIZE);
    player_reset(player);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        dt = Clamp(dt, 0.0f, 0.1f);
        player_update(player, *world, dt);

        const float sensitivity = 0.05f;
        camera.target = player.pos;
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            camera.zoom += GetMouseWheelMove() * sensitivity;
            camera.zoom = Clamp(camera.zoom, sensitivity, 1.0f);
        }

        if (IsKeyPressed(KEY_R))
        {
            world_generate(*world);

            player.spawnPos = Vector2Scale(world->spawn, TILE_SIZE);
            player_reset(player);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        world_draw(*world);
        player_draw(player);

        EndMode2D();

        inventory_draw(player.inventory);
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, SCREEN_HEIGHT - 30, 20, WHITE);

        EndDrawing();
    }

    arena_free(arena);

    CloseWindow();
    return 0;
}