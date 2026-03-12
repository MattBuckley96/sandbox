#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "config.hpp"
#include "world.hpp"

////////////////////////////////////////////////////////////////////////////////

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
    float mineTimer;
    float mineSpeed;

    bool flipY;

    Vector2 spawnPos;

    // TODO: move this
    Texture& texture;
};

////////////////////////////////////////////////////////////////////////////////

Rectangle player_hitbox(Player& player);
void player_update(Player& player, World& world, float dt);
void player_draw(Player& player);
void player_reset(Player& player);