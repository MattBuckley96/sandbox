#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "config.hpp"

////////////////////////////////////////////////////////////////////////////////

enum Block
{
    BLOCK_AIR,
    BLOCK_DIRT,
    BLOCK_GRASS,
    BLOCK_STONE,

    BLOCK_COUNT
};

enum Item
{
    ITEM_NONE,
    ITEM_DIRT_BLOCK,
    ITEM_GRASS_BLOCK,
    ITEM_STONE_BLOCK,

    ITEM_COUNT,
};

struct ItemStack
{
    Item item;
    int count;
};

struct BlockInfo
{
    const char* name;
    int health;
    Item drop;
};

constexpr static BlockInfo blockInfo[BLOCK_COUNT]
{
    // .name  .health   .drop
    { "air",   0,      ITEM_NONE },
    { "dirt",  2,      ITEM_DIRT_BLOCK },
    { "grass", 2,      ITEM_GRASS_BLOCK },
    { "stone", 3,      ITEM_STONE_BLOCK },
};

struct ItemInfo
{
    const char* name;
};

constexpr static ItemInfo itemInfo[ITEM_COUNT]
{
    // .name
    { "null" },
    { "dirt block" },
    { "grass block" },
    { "stone block" },
};

struct World
{
    Block blocks[WORLD_WIDTH][WORLD_HEIGHT];
    int blockHealth[WORLD_WIDTH][WORLD_HEIGHT];

    RenderTexture blockMap;
    RenderTexture lightMap;
    Vector2 spawn;

    // TODO: move these
    Camera2D& camera;
    Texture& atlas;
    Texture& lightTexture;
    Texture& blockBreakTexture;
};

////////////////////////////////////////////////////////////////////////////////

void world_update_light(World& world);
void world_set_block(World& world, int x, int y, Block block);
void world_generate(World& world);
void world_draw(World& world);