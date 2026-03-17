#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "config.hpp"

////////////////////////////////////////////////////////////////////////////////

enum BlockType
{
    BLOCK_TYPE_SOLID,
    BLOCK_TYPE_NOCLIP,
};

enum Block
{
    BLOCK_AIR,
    BLOCK_DIRT,
    BLOCK_GRASS,
    BLOCK_STONE,
    BLOCK_PLANKS,

    // TODO: trees probably should be walls in future
    BLOCK_LOG,
    BLOCK_LEAVES,

    BLOCK_COUNT
};

enum ItemType
{
    ITEM_TYPE_BLOCK,
    ITEM_TYPE_TOOL,
};

enum Item
{
    ITEM_NONE,

    // blocks
    ITEM_DIRT_BLOCK,
    ITEM_GRASS_BLOCK,
    ITEM_STONE_BLOCK,
    ITEM_PLANKS,

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
    BlockType type;
    Item drop;
};

constexpr static BlockInfo blockInfo[BLOCK_COUNT]
{
    // .name  .health       .type           .drop
    { "air",     0,     BLOCK_TYPE_NOCLIP, ITEM_NONE },
    { "dirt",    2,     BLOCK_TYPE_SOLID,  ITEM_DIRT_BLOCK },
    { "grass",   2,     BLOCK_TYPE_SOLID,  ITEM_GRASS_BLOCK },
    { "stone",   3,     BLOCK_TYPE_SOLID,  ITEM_STONE_BLOCK },
    { "planks",  3,     BLOCK_TYPE_SOLID,  ITEM_PLANKS },
    { "log",     8,    BLOCK_TYPE_NOCLIP, ITEM_PLANKS },
    { "leaves",  0,     BLOCK_TYPE_NOCLIP, ITEM_NONE },
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
    { "planks",      ITEM_TYPE_BLOCK },
    { "pickaxe",     ITEM_TYPE_TOOL },
};

struct World
{
    Block blocks[WORLD_WIDTH][WORLD_HEIGHT];
    int blockHealth[WORLD_WIDTH][WORLD_HEIGHT];

    RenderTexture blockMap;
    Vector2 spawn;

    // TODO: move these
    Camera2D& camera;
};

////////////////////////////////////////////////////////////////////////////////

void block_draw(Block block, Rectangle dest);
void item_draw(Item item, Rectangle dest);

void world_set_block(World& world, int x, int y, Block block);
void world_generate(World& world);
void world_draw(World& world);