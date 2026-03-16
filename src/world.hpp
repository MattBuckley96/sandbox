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

    // blocks
    ITEM_DIRT_BLOCK,
    ITEM_GRASS_BLOCK,
    ITEM_STONE_BLOCK,

    // tools
    ITEM_PICKAXE,

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
    // .name  .health     .drop
    { "air",     0,    ITEM_NONE },
    { "dirt",    2,    ITEM_DIRT_BLOCK },
    { "grass",   2,    ITEM_GRASS_BLOCK },
    { "stone",   3,    ITEM_STONE_BLOCK },
};

enum ItemType
{
    ITEM_TYPE_BLOCK,
    ITEM_TYPE_TOOL,
};

struct ItemInfo
{
    const char* name;
    ItemType type;
};

constexpr static ItemInfo itemInfo[ITEM_COUNT]
{
    // .name              .type
    { "null",        ITEM_TYPE_BLOCK },
    { "dirt block",  ITEM_TYPE_BLOCK },
    { "grass block", ITEM_TYPE_BLOCK },
    { "stone block", ITEM_TYPE_BLOCK },
    { "pickaxe",     ITEM_TYPE_TOOL },
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
    Texture& lightTexture;
    Texture& blockBreakTexture;
};

////////////////////////////////////////////////////////////////////////////////

void block_draw(Block block, Rectangle dest);
void item_draw(Item item, Rectangle dest);

void world_update_light(World& world);
void world_set_block(World& world, int x, int y, Block block);
void world_generate(World& world);
void world_draw(World& world);