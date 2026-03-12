#include <algorithm>

#include "world.hpp"

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