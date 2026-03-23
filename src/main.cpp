#include <cmath>

#include "config.hpp"
#include "world.hpp"
#include "player.hpp"

////////////////////////////////////////////////////////////////////////////////

Texture blockAtlas; 
Texture itemAtlas;
Texture playerTexture; 
Texture breakTexture; 
Texture treeTexture;
Texture uiTexture;

Arena arena;
Player player;
Camera2D camera;
World* world;

////////////////////////////////////////////////////////////////////////////////

void game_init()
{
    player = {
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

    camera = {
        .offset = screenCenter,
        .target = player.pos,
        .zoom = 1.0f,
    };

    if (world == nullptr)
    {
        world = (World*)arena_alloc(arena, sizeof(World));
        if (world == nullptr)
        {
            std::exit(-1);
        }
    }

    world_generate(*world);

    player.spawnPos = Vector2Scale(world->spawn, TILE_SIZE);
    player_reset(player);
}

void game_update()
{
    if (IsKeyPressed(KEY_R))
    {
        game_init();
    }

    float dt = GetFrameTime();
    dt = Clamp(dt, 0.0f, 0.1f);

    player_update(player, *world, dt);

    const float sensitivity = 0.2f;
    camera.target.x = std::roundf(player.pos.x);
    camera.target.y = std::roundf(player.pos.y);
    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        camera.zoom += GetMouseWheelMove() * sensitivity;
        camera.zoom = Clamp(camera.zoom, sensitivity, 1.0f);
    }
}

void game_draw()
{
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

////////////////////////////////////////////////////////////////////////////////

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sandbox");

    blockAtlas = LoadTexture("assets/blocks.png");
    itemAtlas = LoadTexture("assets/items.png");
    playerTexture = LoadTexture("assets/player.png");
    breakTexture = LoadTexture("assets/break.png");
    treeTexture = LoadTexture("assets/tree.png");
    uiTexture = LoadTexture("assets/ui.png");

    arena_init(arena, sizeof(World));
    game_init();

    while (!WindowShouldClose())
    {
        game_update();
        game_draw();
    }

    arena_free(arena);

    CloseWindow();
    return 0;
}