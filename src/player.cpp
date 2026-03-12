#include <cmath>
#include <algorithm>

#include "player.hpp"

////////////////////////////////////////////////////////////////////////////////

Rectangle player_hitbox(Player& player)
{
    const float scale = (TILE_SIZE / 8);

    const Vector2 size = {
        (float)player.texture.width * scale,
        (float)player.texture.height * scale
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

    if (player.mineTimer > 0.0f)
    {
        player.mineTimer -= dt;
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
    player.inventory = {};
}

void inventory_add(Inventory& inventory, ItemStack& stack)
{
    int slotIdx = -1;

    // check for existing
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        ItemStack slot = inventory.slots[i];

        if (slot.item == stack.item)
        {
            slotIdx = i;
            break;
        }
    }

    // check for free
    if (slotIdx == -1)
    {
        for (int i = 0; i < INVENTORY_SIZE; i++)
        {
            ItemStack& slot = inventory.slots[i];

            if (slot.item == ITEM_NONE)
            {
                slot.item = stack.item;
                slotIdx = i;
                break;
            }
        }
    }

    if (slotIdx == -1)
    {
        return;
    }

    ItemStack& slot = inventory.slots[slotIdx];
    slot.count += stack.count;

    if (slot.count > MAX_STACK)
    {
        slot.count = MAX_STACK;
    }
}

void inventory_draw(Inventory& inventory)
{
    const float slotSize = TILE_SIZE * 1.5f;

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        Rectangle slotRect = {
            10 + (i * slotSize),
            10,
            slotSize,
            slotSize
        };
        DrawRectangleRec(slotRect, Fade(BLACK, 0.5f));
        DrawRectangleLinesEx(slotRect, 2.0f, WHITE);

        ItemStack slot = inventory.slots[i];
        if (slot.item == ITEM_NONE)
        {
            continue;
        }

        Rectangle itemRect = {
            slotRect.x + (slotSize / 4),
            slotRect.y + (slotSize / 4),
            (slotSize / 2),
            (slotSize / 2)
        };

        Color color;
        switch (slot.item)
        {
            case ITEM_DIRT_BLOCK:
            {
                color = BROWN;
                break;
            }

            case ITEM_GRASS_BLOCK:
            {
                color = GREEN;
                break;
            }

            case ITEM_STONE_BLOCK:
            {
                color = GRAY;
                break;
            }

            default:
            {
                continue;
            }
        }

        DrawRectangleRec(itemRect, color);

        if (slot.count == 0)
        {
            continue;
        }

        int fontSize = 18;
        const char* text = TextFormat("%i", slot.count);
        int textWidth = MeasureText(text, fontSize);

        int textX = (slotRect.x + (slotRect.width / 2)) - (textWidth / 2);
        int textY = (slotRect.y + slotRect.height) - (fontSize);
        DrawText(text, textX, textY, fontSize, WHITE);
    }
}