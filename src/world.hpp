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