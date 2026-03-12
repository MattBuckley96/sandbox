#include <cmath>
#include <algorithm>

#include <raylib.h>
#include <raymath.h>

////////////////////////////////////////////////////////////////////////////////

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
const Vector2 screenCenter = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

#define TILE_SIZE 32
#define WORLD_WIDTH 200
#define WORLD_HEIGHT 100

////////////////////////////////////////////////////////////////////////////////

enum Block
{
    BLOCK_AIR,
    BLOCK_DIRT,
    BLOCK_GRASS,
    BLOCK_STONE,

    // TODO: may need in future
    BLOCK_COUNT
};

struct World
{
    Block blocks[WORLD_WIDTH][WORLD_HEIGHT];
    RenderTexture blockMap;
    RenderTexture lightMap;
    Vector2 spawn;

    // TODO: move these?
    Camera2D& camera;
    Texture& atlas;
    Texture& lightTexture;
};

struct Player
{
    Vector2 pos;
    Vector2 vel;
    float accel;
    float friction;
    float maxSpeed;
    float gravity;
    float jumpForce;

    float jumpBufferTimer;
    float jumpBuffer;
    float jumpHoldTimer;
    float jumpHoldTimePrev;
    float coyoteTimer;
    float coyoteBuffer;

    bool flipY;

    Vector2 spawnPos;

    // TODO: move this
    Texture& texture;
};

////////////////////////////////////////////////////////////////////////////////

void world_update_light(World& world)
{
    return;

    /*
        TODO: this is horribly inefficient...
        - may want to do some chunk-based thing?
    */
    BeginTextureMode(world.lightMap);
    ClearBackground(BLACK);
    BeginBlendMode(BLEND_ADDITIVE);

    for (int y = 0; y < WORLD_HEIGHT; y++)
    {
        for (int x = 0; x < WORLD_WIDTH; x++)
        {
            Block block = world.blocks[x][y];

            if (block == BLOCK_AIR)
            {
                Vector2 pos = { (float)x, (float)y };
                Vector2 offset = { world.lightTexture.width / 2.0f, world.lightTexture.height / 2.0f };
                pos = Vector2Subtract(pos, offset);

                DrawTextureEx(world.lightTexture, pos, 0.0f, 1.0f, WHITE);
            }
        }
    }

    EndBlendMode();
    EndTextureMode();
}

void world_generate(World& world)
{
    for (int y = 0; y < WORLD_HEIGHT; y++)
    {
        for (int x = 0; x < WORLD_WIDTH; x++)
        {
            world.blocks[x][y] = BLOCK_AIR;
        }
    }

    int height = WORLD_HEIGHT * 0.5f;
    const int dirtHeight = 10;

    for (int x = 0; x < WORLD_WIDTH; x++)
    {
        if (GetRandomValue(1, 2) == 1)
        {
            height += GetRandomValue(-1, 1);
        }

        for (int y = height; y < WORLD_HEIGHT; y++)
        {
            world.blocks[x][y] = BLOCK_STONE;
        }

        int y;
        for (y = (height + dirtHeight); y >= height; y--)
        {
            world.blocks[x][y] = BLOCK_DIRT;
        }
        world.blocks[x][y] = BLOCK_GRASS;

        // TODO: make actual spawn placement logic
        if (x == (int)(WORLD_WIDTH / 2))
        {
            world.spawn = { (float)x, (float)y - 1 };
        }
    }

    world_update_light(world);
}

void world_draw(World& world)
{
    /*
        TODO: redraw the world only on update 
    */

    EndMode2D();
    BeginTextureMode(world.blockMap);

    const Rectangle skyRect = {
        0,
        0,
        WORLD_WIDTH * TILE_SIZE,
        WORLD_HEIGHT * TILE_SIZE
    };
    const Color skyTop = { 18, 78, 137, 255 };
    const Color skyBottom = { 0, 153, 219, 255};

    /*
        TODO: raylib has a bug where top-right and bottom-right are
        flipped, update this when raylib fixes
    */
    DrawRectangleGradientEx(skyRect, skyTop, skyBottom, skyBottom, skyTop);

    const Vector2 screenBounds = { SCREEN_WIDTH, SCREEN_HEIGHT };
    Vector2 viewStart = GetScreenToWorld2D({ 0, 0 }, world.camera);
    Vector2 viewEnd = GetScreenToWorld2D(screenBounds, world.camera);

    int xStart = std::max(0, (int)(viewStart.x / TILE_SIZE) - 1);
    int yStart = std::max(0, (int)(viewStart.y / TILE_SIZE) - 1);

    int xEnd = std::min(WORLD_WIDTH, (int)(viewEnd.x / TILE_SIZE) + 2);
    int yEnd = std::min(WORLD_HEIGHT, (int)(viewEnd.y / TILE_SIZE) + 2);

    for (int y = yStart; y < yEnd; y++)
    {
        for (int x = xStart; x < xEnd; x++)
        {
            Block block = world.blocks[x][y];
            if (block == BLOCK_AIR)
            {
                continue;
            }

            Rectangle source = {};
            switch (block)
            {
                case BLOCK_DIRT:
                {
                    source = { 8, 0, 8, 8 };
                    break;
                }

                case BLOCK_GRASS:
                {
                    source = { 16, 0, 8, 8 };
                    break;
                }

                case BLOCK_STONE:
                {
                    source = { 24, 0, 8, 8 };
                    break;
                }

                default:
                {
                    continue;
                }
            }

            Rectangle dest = {
                (float)x * TILE_SIZE,
                (float)y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };
            DrawTexturePro(world.atlas, source, dest, { 0, 0 }, 0.0f, WHITE);

            if (x == world.spawn.x && y == world.spawn.y)
            {
                DrawRectangleRec(dest, GREEN);
            }
        }
    }

    EndTextureMode();
    BeginMode2D(world.camera);

    Rectangle worldDest = {
        0,
        0,
        WORLD_WIDTH * TILE_SIZE,
        WORLD_HEIGHT * TILE_SIZE
    };

    Rectangle blockSource = {
        0,
        0,
        (float)world.blockMap.texture.width,
        (float)-world.blockMap.texture.height
    };

    DrawTexturePro(world.blockMap.texture, blockSource, worldDest, 
        { 0, 0 }, 0.0f, WHITE);

    // NOTE: needs negative height to flip
    Rectangle lightSource = {
        0,
        0,
        (float)world.lightMap.texture.width,
        (float)-world.lightMap.texture.height,
    };

    /*
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawTexturePro(world.lightMap.texture, lightSource, worldDest, 
        { 0, 0 }, 0.0f, WHITE);
    EndBlendMode();
    */
}

Rectangle player_hitbox(Player& player)
{
    const Vector2 size = {
        (float)player.texture.width * 4,
        (float)player.texture.height * 4
    };
    Rectangle hitbox = {
        player.pos.x - (size.x / 2),
        player.pos.y - (size.y / 2),
        size.x,
        size.y
    };
    return hitbox;
}

void player_update(Player& player, World& world, float dt)
{
    // timers
    if (player.coyoteTimer > 0.0f)
    {
        player.coyoteTimer -= dt;
    }

    if (player.jumpBufferTimer > 0.0f)
    {
        player.jumpBufferTimer -= dt;
    }

    // x
    int dir = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    float amount = 0.0f;

    if (dir == 1)
    {
        amount = player.accel * dt;
        player.flipY = true;
    }
    else if (dir == -1)
    {
        amount = player.accel * dt;
        player.flipY = false;
    }
    else
    {
        amount = player.friction * dt;
    }

    player.vel.x = Lerp(player.vel.x, dir * player.maxSpeed, amount);

    // y
    player.vel.y += player.gravity * dt;

    if (IsKeyDown(KEY_SPACE))
    {
        player.jumpHoldTimer += dt;
        player.jumpHoldTimePrev = player.jumpHoldTimer;
    }
    else
    {
        player.jumpHoldTimer = 0.0f;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        player.jumpBufferTimer = player.jumpBuffer;
    }

    if (player.coyoteTimer > 0.0f && player.jumpBufferTimer > 0.0f)
    {
        player.vel.y = -player.jumpForce;
        player.jumpBufferTimer = 0.0f;
    }

    bool jumpReleased = (player.jumpHoldTimer < player.jumpHoldTimePrev);
    if (jumpReleased && player.vel.y < 0.0f)
    {
        player.vel.y *= 0.4f;
        player.jumpHoldTimePrev = player.jumpHoldTimer;
    }

    player.pos = Vector2Add(player.pos, Vector2Scale(player.vel, dt));

    // collision
    Rectangle hitbox = player_hitbox(player);

    int px = std::roundf(player.pos.x / TILE_SIZE);
    int py = std::roundf(player.pos.y / TILE_SIZE);

    for (int y = py - 2; y < py + 2; y++)
    {
        for (int x = px - 2; x < px + 2; x++)
        {
            if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT)
            {
                continue;
            }

            Block block = world.blocks[x][y];
            if (block == BLOCK_AIR)
            {
                continue;
            }

            Rectangle tile = {
                (float)x * TILE_SIZE,
                (float)y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };

            if (!CheckCollisionRecs(hitbox, tile))
            {
                continue;
            }

            float xOverlap = 0.0f;
            float yOverlap = 0.0f;
            
            float left = tile.x - (hitbox.x + hitbox.width);
            float right = (tile.x + tile.width) - hitbox.x;
            float top = tile.y - (hitbox.y + hitbox.height);
            float bottom = (tile.y + tile.height) - hitbox.y;

            if (std::fabs(left) < right)
            {
                xOverlap = left;
            }
            else
            {
                xOverlap = right;
            }

            if (std::fabs(top) < bottom)
            {
                yOverlap = top;
            }
            else
            {
                yOverlap = bottom;
            }

            if (std::fabs(xOverlap) < std::fabs(yOverlap))
            {
                player.pos.x += xOverlap;
                player.vel.x = 0.0f;
            }
            else
            {
                player.pos.y += yOverlap;
                player.vel.y = 0.0f;
            }
        }
    }

    if (player.vel.y == 0.0f)
    {
        player.coyoteTimer = player.coyoteBuffer;
    }
}

void player_draw(Player& player)
{
    Rectangle source = {
        0,
        0,
        (float)player.texture.width,
        (float)player.texture.height
    };
    Rectangle dest = player_hitbox(player);

    if (player.flipY)
    {
        source.width = -source.width;
    }

    DrawTexturePro(player.texture, source, dest, { 0, 0 }, 0.0f, WHITE);
}

void player_reset(Player& player)
{
    player.pos = player.spawnPos;
    player.vel = {};
    player.coyoteTimer = 0.0f;
    player.jumpBufferTimer = 0.0f;
    player.jumpHoldTimer = 0.0f;
}

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