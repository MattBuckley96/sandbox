#pragma once

////////////////////////////////////////////////////////////////////////////////

#include <raylib.h>
#include <raymath.h>

////////////////////////////////////////////////////////////////////////////////

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
const Vector2 screenCenter = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

#define TILE_SIZE 32
#define WORLD_WIDTH 200
#define WORLD_HEIGHT 100

extern Texture blockAtlas; 
extern Texture lightTexture; 
extern Texture playerTexture; 
extern Texture blockBreakTexture; 