#include "config.hpp"
#include "world.hpp"
#include "player.hpp"

////////////////////////////////////////////////////////////////////////////////

int main()
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sandbox");

    Texture atlas = LoadTexture("assets/atlas.png");
    Texture lightTexture = LoadTexture("assets/light.png");
    Texture playerTexture = LoadTexture("assets/player.png");

    Player player = {
        .accel = 9.0f,
        .friction = 10.0f,
        .maxSpeed = 300.0f,
        .gravity = 860.0f,
        .jumpForce = 465.0f,
        .jumpBuffer = 0.2f,
        .coyoteBuffer = 0.1f,
        .texture = playerTexture,
    };

    Camera2D camera = {
        .offset = screenCenter,
        .target = player.pos,
        .zoom = 1.0f,
    };

    World world = {
        .lightMap = LoadRenderTexture(WORLD_WIDTH, WORLD_HEIGHT),
        .camera = camera,
        .atlas = atlas,
        .lightTexture = lightTexture,
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
        camera.zoom += GetMouseWheelMove() * sensitivity;
        camera.zoom = Clamp(camera.zoom, sensitivity, 1.0f);

        if (IsKeyPressed(KEY_R))
        {
            world_generate(world);

            player.spawnPos = Vector2Scale(world.spawn, TILE_SIZE);
            player_reset(player);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos = GetMousePosition();
            mousePos = GetScreenToWorld2D(mousePos, camera);
            int x = (int)(mousePos.x / TILE_SIZE);
            int y = (int)(mousePos.y / TILE_SIZE);

            if (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT)
            {
                world.blocks[x][y] = BLOCK_AIR;
                world_update_light(world);
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        world_draw(world);
        player_draw(player);

        EndMode2D();

        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}