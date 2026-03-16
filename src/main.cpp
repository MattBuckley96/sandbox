#include "config.hpp"
#include "world.hpp"
#include "player.hpp"

////////////////////////////////////////////////////////////////////////////////

Texture blockAtlas; 
Texture itemAtlas;
Texture lightTexture; 
Texture playerTexture; 
Texture blockBreakTexture; 

////////////////////////////////////////////////////////////////////////////////

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sandbox");

    blockAtlas = LoadTexture("assets/block_atlas.png");
    itemAtlas = LoadTexture("assets/item_atlas.png");
    lightTexture = LoadTexture("assets/light.png");
    playerTexture = LoadTexture("assets/player.png");
    blockBreakTexture = LoadTexture("assets/block_break.png");

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

    // TODO: put the world on the heap, its way to damn big
    World world = {
        .lightMap = LoadRenderTexture(WORLD_WIDTH, WORLD_HEIGHT),
        .camera = camera,
        .lightTexture = lightTexture,
        .blockBreakTexture = blockBreakTexture,
    };
    world.blockMap = LoadRenderTexture(
        WORLD_WIDTH * TILE_SIZE,
        WORLD_HEIGHT * TILE_SIZE
    );
    world_generate(world);

    player.spawnPos = Vector2Scale(world.spawn, TILE_SIZE);
    player_reset(player);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        dt = Clamp(dt, 0.0f, 0.1f);
        player_update(player, world, dt);

        const float sensitivity = 0.05f;
        camera.target = player.pos;
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            camera.zoom += GetMouseWheelMove() * sensitivity;
            camera.zoom = Clamp(camera.zoom, sensitivity, 1.0f);
        }

        if (IsKeyPressed(KEY_R))
        {
            world_generate(world);

            player.spawnPos = Vector2Scale(world.spawn, TILE_SIZE);
            player_reset(player);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        world_draw(world);
        player_draw(player);

        EndMode2D();

        inventory_draw(player.inventory);
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, SCREEN_HEIGHT - 30, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}