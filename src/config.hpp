#pragma once

////////////////////////////////////////////////////////////////////////////////

#include <raylib.h>
#include <raymath.h>
#include <memory>

////////////////////////////////////////////////////////////////////////////////

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
const Vector2 screenCenter = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

#define TILE_SIZE 32
#define WORLD_WIDTH 800
#define WORLD_HEIGHT 400

// TODO: move these
extern Texture blockAtlas; 
extern Texture itemAtlas;
extern Texture playerTexture; 
extern Texture breakTexture; 
extern Texture treeTexture;
extern Texture uiTexture;

////////////////////////////////////////////////////////////////////////////////

#define KiB(x) (x * 1024)
#define MiB(x) (KiB(x) * 1024)
#define GiB(x) (MiB(x) * 1024)

struct Arena
{
    size_t size;
    size_t capacity;
    char* memory;
};

static void arena_init(Arena& arena, size_t capacity)
{
    arena.size = 0;
    arena.capacity = capacity;

    arena.memory = (char*)malloc(capacity);
    if (arena.memory)
    {
        memset(arena.memory, 0, capacity);
    }
}

static void arena_free(Arena& arena)
{
    free(arena.memory);
}

static void* arena_alloc(Arena& arena, size_t size)
{
    void* ptr = nullptr;

    size = (size + 7) & ~7;
    if ((arena.size + size) <= arena.capacity)
    {
        ptr = arena.memory + arena.size;
        arena.size += size;
    }

    return ptr;
}

////////////////////////////////////////////////////////////////////////////////