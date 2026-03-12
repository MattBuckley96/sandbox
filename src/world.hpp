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

////////////////////////////////////////////////////////////////////////////////

void world_update_light(World& world);
void world_generate(World& world);
void world_draw(World& world);