#include <cmath>
#include <algorithm>

#include "player.hpp"

////////////////////////////////////////////////////////////////////////////////

Rectangle player_hitbox(Player& player)
{
    const Vector2 size = {
        (float)player.texture.width * 4,
        (float)player.texture.height * 4
    };
    Rectangle hitbox = {
        player.pos.x - (size.x / 2),
        player.pos.y - (size.y / 2),
        size.x,
        size.y
    };
    return hitbox;
}

void player_update(Player& player, World& world, float dt)
{
    // timers
    if (player.coyoteTimer > 0.0f)
    {
        player.coyoteTimer -= dt;
    }

    if (player.jumpBufferTimer > 0.0f)
    {
        player.jumpBufferTimer -= dt;
    }

    // x
    int dir = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    float amount = 0.0f;

    if (dir == 1)
    {
        amount = player.accel * dt;
        player.flipY = true;
    }
    else if (dir == -1)
    {
        amount = player.accel * dt;
        player.flipY = false;
    }
    else
    {
        amount = player.friction * dt;
    }

    player.vel.x = Lerp(player.vel.x, dir * player.maxSpeed, amount);

    // y
    player.vel.y += player.gravity * dt;

    if (IsKeyDown(KEY_SPACE))
    {
        player.jumpHoldTimer += dt;
        player.jumpHoldTimePrev = player.jumpHoldTimer;
    }
    else
    {
        player.jumpHoldTimer = 0.0f;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        player.jumpBufferTimer = player.jumpBuffer;
    }

    if (player.coyoteTimer > 0.0f && player.jumpBufferTimer > 0.0f)
    {
        player.vel.y = -player.jumpForce;
        player.jumpBufferTimer = 0.0f;
    }

    bool jumpReleased = (player.jumpHoldTimer < player.jumpHoldTimePrev);
    if (jumpReleased && player.vel.y < 0.0f)
    {
        player.vel.y *= 0.4f;
        player.jumpHoldTimePrev = player.jumpHoldTimer;
    }

    player.pos = Vector2Add(player.pos, Vector2Scale(player.vel, dt));

    // collision
    Rectangle hitbox = player_hitbox(player);

    int px = std::roundf(player.pos.x / TILE_SIZE);
    int py = std::roundf(player.pos.y / TILE_SIZE);

    for (int y = py - 2; y < py + 2; y++)
    {
        for (int x = px - 2; x < px + 2; x++)
        {
            if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT)
            {
                continue;
            }

            Block block = world.blocks[x][y];
            if (block == BLOCK_AIR)
            {
                continue;
            }

            Rectangle tile = {
                (float)x * TILE_SIZE,
                (float)y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };

            if (!CheckCollisionRecs(hitbox, tile))
            {
                continue;
            }

            float xOverlap = 0.0f;
            float yOverlap = 0.0f;
            
            float left = tile.x - (hitbox.x + hitbox.width);
            float right = (tile.x + tile.width) - hitbox.x;
            float top = tile.y - (hitbox.y + hitbox.height);
            float bottom = (tile.y + tile.height) - hitbox.y;

            if (std::fabs(left) < right)
            {
                xOverlap = left;
            }
            else
            {
                xOverlap = right;
            }

            if (std::fabs(top) < bottom)
            {
                yOverlap = top;
            }
            else
            {
                yOverlap = bottom;
            }

            if (std::fabs(xOverlap) < std::fabs(yOverlap))
            {
                player.pos.x += xOverlap;
                player.vel.x = 0.0f;
            }
            else
            {
                player.pos.y += yOverlap;
                player.vel.y = 0.0f;
            }
        }
    }

    if (player.vel.y == 0.0f)
    {
        player.coyoteTimer = player.coyoteBuffer;
    }
}

void player_draw(Player& player)
{
    Rectangle source = {
        0,
        0,
        (float)player.texture.width,
        (float)player.texture.height
    };
    Rectangle dest = player_hitbox(player);

    if (player.flipY)
    {
        source.width = -source.width;
    }

    DrawTexturePro(player.texture, source, dest, { 0, 0 }, 0.0f, WHITE);
}

void player_reset(Player& player)
{
    player.pos = player.spawnPos;
    player.vel = {};
    player.coyoteTimer = 0.0f;
    player.jumpBufferTimer = 0.0f;
    player.jumpHoldTimer = 0.0f;
}
