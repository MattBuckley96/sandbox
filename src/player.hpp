#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "config.hpp"
#include "world.hpp"

////////////////////////////////////////////////////////////////////////////////

#define INVENTORY_WIDTH 10
#define INVENTORY_HEIGHT 4
#define INVENTORY_SIZE (INVENTORY_WIDTH * INVENTORY_HEIGHT)

struct Inventory
{
    ItemStack slots[INVENTORY_SIZE];
    ItemStack handSlot;
    bool open;
    bool hovered;
    int selectedIdx;
};

struct Player
{
    Vector2 pos;
    Vector2 spawnPos;
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
    float placeTimer;
    float placeSpeed;

    bool flipY;

    Inventory inventory;
};

////////////////////////////////////////////////////////////////////////////////

Rectangle player_hitbox(Player& player);
void player_update(Player& player, World& world, float dt);
void player_draw(Player& player);
void player_reset(Player& player);

void inventory_update(Inventory& inventory, float dt);
void inventory_add(Inventory& inventory, ItemStack& stack);
void inventory_draw(Inventory& inventory);