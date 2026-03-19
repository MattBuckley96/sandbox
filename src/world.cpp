#include <cmath>
#include <algorithm>

#include "world.hpp"

////////////////////////////////////////////////////////////////////////////////

void block_draw(Block block, Rectangle dest)
{
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

        case BLOCK_PLANKS:
        {
            source = { 32, 0, 8, 8 };
            break;
        }

        case BLOCK_COAL_ORE:
        {
            source = { 40, 0, 8, 8 };
            break;
        }

        case BLOCK_LOG:
        {
            source = { 24, 10, 8, 8 };
            DrawTexturePro(treeTexture, source, dest, { 0, 0 }, 0.0f, WHITE);
            return;
        }

        case BLOCK_LEAVES:
        {
            source = { 0, 0, 23, 18 };
            DrawTexturePro(treeTexture, source, dest, { 0, 0 }, 0.0f, WHITE);
            return;
        }

        default:
        {
            return;
        }
    }
    DrawTexturePro(blockAtlas, source, dest, { 0, 0 }, 0.0f, WHITE);
}

void item_draw(Item item, Rectangle dest)
{
    Rectangle source = {};
    switch (itemInfo[item].type)
    {
        case ITEM_TYPE_BLOCK:
        {
            block_draw((Block)item, dest);
            return;
        }

        default:
        {
            switch (item)
            {
                case ITEM_PICKAXE:
                {
                    source = { 16, 0, 16, 16 };
                    break;
                }

                case ITEM_COAL:
                {
                    source = { 32, 0, 8, 8 };
                    break;
                }

                default:
                {
                    return;
                }
            }
        }
    }

    DrawTexturePro(itemAtlas, source, dest, { 0, 0 }, 0.0f, WHITE);
}

void world_set_block(World& world, int x, int y, Block block)
{
    if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT)
    {
        return;
    }

    world.blocks[x][y] = block;
    world.blockHealth[x][y] = blockInfo[block].health;
}

void world_generate(World& world)
{
    for (int y = 0; y < WORLD_HEIGHT; y++)
    {
        for (int x = 0; x < WORLD_WIDTH; x++)
        {
            world_set_block(world, x, y, BLOCK_AIR);
        }
    }

    int height = WORLD_HEIGHT * 0.5f;
    const int dirtHeight = 10;
    int treeDist = 3;

    for (int x = 0; x < WORLD_WIDTH; x++)
    {
        if (GetRandomValue(1, 2) == 1)
        {
            height += GetRandomValue(-1, 1);
        }

        for (int y = height; y < WORLD_HEIGHT; y++)
        {
            world_set_block(world, x, y, BLOCK_STONE);

            // coal
            if (y >= WORLD_HEIGHT * 0.7f)
            {
                if (GetRandomValue(1, 30) == 1)
                {
                    int oreCount = GetRandomValue(5, 10);
                    int oreX = x;
                    int oreY = y;

                    while (oreCount > 0)
                    {
                        oreX += GetRandomValue(-1, 1);    
                        oreY += GetRandomValue(-1, 1);
                        world_set_block(world, oreX, oreY, BLOCK_COAL_ORE);
                        oreCount--;
                    }
                }
            }
        }

        int y;
        for (y = (height + dirtHeight); y >= height; y--)
        {
            world_set_block(world, x, y, BLOCK_DIRT);
        }
        world_set_block(world, x, y, BLOCK_GRASS);

        // trees
        if (treeDist > 2 && GetRandomValue(1, 15) == 1)
        {
            int treeHeight = GetRandomValue(10, 15);

            for (int i = 1; i <= treeHeight; i++)
            {
                world_set_block(world, x, y - i, BLOCK_LOG);
            }
            world_set_block(world, x, y - treeHeight, BLOCK_LEAVES);

            treeDist = 0;
        }

        // TODO: make actual spawn placement logic
        if (x == (int)(WORLD_WIDTH / 2))
        {
            world.spawn = { (float)x, (float)y - 1 };
        }

        treeDist++;
    }
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

            Rectangle dest = {
                (float)x * TILE_SIZE,
                (float)y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };

            // my god
            if (block == BLOCK_LEAVES)
            {
                const float scale = (TILE_SIZE / 8);
                const float width = 23 * scale;
                const float height = 18 * scale;
                dest = {
                    (float)(x * TILE_SIZE) - (width / 2) + (TILE_SIZE / 2) + (scale / 2),
                    (float)(y * TILE_SIZE) - TILE_SIZE - (scale * 2),
                    width,
                    height
                };
                DrawCircle(x * TILE_SIZE, y * TILE_SIZE, 2, RED);
            }
            block_draw(block, dest);

            int health = world.blockHealth[x][y];
            int maxHealth = blockInfo[block].health;
            if (health < maxHealth)
            {
                const int frameCount = 3;
                int frame = ((float)(maxHealth - health) / maxHealth) * frameCount;
                Rectangle source = { (float)frame * 8, 0, 8, 8 };
                DrawTexturePro(blockBreakTexture, source, dest, { 0, 0 }, 0.0f, WHITE);
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
}