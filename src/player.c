/******************************************************************************
* @filename		player.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file defines the function and logic for player movement,
                taking damage and rendering.
******************************************************************************/

#include "cprocessing.h"
#include "player.h"
#include "utils.h"
#include "camera.h"
#include "flicker.h"
#include "level1.h"
#include "placeables.h"
#include "audio.h"

#include <stdio.h>
#include <stdbool.h>

static const float DASH_COOLDOWN = 1.f,
    DASH_DISTANCE = 200.f, DASH_SPEED = 15.f;
static const float WALK_SOUND_INTERVAL = 0.5f;

static bool _isDashing = false;
static float _currentDashTime, _dashCooldownTimer;
static CP_Vector _dashTo;
static float _walkSoundTimer;

struct ChocoShooter playerInstance; // Global variable, not private

// Returns 1 if still alive, 0 if not
// Made by Dalton Koh
int player_take_damage(struct ChocoShooter* player, int damage) {
    if (player->flickerState.isFlickering) {
        //printf("chocoshooter is invincible and cannot take damage right now.\n");
        return 1;  // Skip the rest of the function if flickering
    }

    flicker(&(player->flickerState), 1);
    player->health = CP_Math_ClampInt(player->health - damage, 0, player->maxHealth);
    play_sound(SOUND_PLAYER_HURT);

    if (player->health > 0) {
        printf("Player took %d damage, health is now %d\n", damage, player->health);
        return 1;
    }

    printf("Choco Shooter has been defeated!\n");
    return 0;
}

struct ChocoShooter init_player(CP_Vector pos, float speed, int maxHealth, int atk, float fireRate, float frameTime) {
    _isDashing = false;
    _dashCooldownTimer = DASH_COOLDOWN + 1.f;
    _dashTo = CP_Vector_Zero();
    _walkSoundTimer = WALK_SOUND_INTERVAL;

    struct ChocoShooter player = {
        .pos = pos,
        .speed = speed,
        .health = maxHealth,
        .maxHealth = maxHealth,
        .img = CP_Image_Load("Assets/choco_shooter.png"),
        .movement = STOPPED,
        .width = 100.f,
        .attackSpeed = fireRate,
        .attackPower = atk,
        .flickerState.isVisible = TRUE,
        .exp = 0,
        .expGained = 0,
        .currentProjectileType = NORMAL_PROJECTILE,
        .isReflectingDmg = false
    };
   
    player.chocoFrames[0] = CP_Image_Load("Assets/Sprites/Player/1.png");
    player.chocoFrames[1] = CP_Image_Load("Assets/Sprites/Player/2.png");
    player.chocoFrames[2] = CP_Image_Load("Assets/Sprites/Player/3.png");
    player.chocoFrames[3] = CP_Image_Load("Assets/Sprites/Player/4.png");
    player.chocoFrames[4] = CP_Image_Load("Assets/Sprites/Player/5.png");
    player.chocoFrames[5] = CP_Image_Load("Assets/Sprites/Player/6.png");

    for (int i = 0; i < CHOCOSHOOTER_FRAMES; i++) {
        if (player.chocoFrames[i] == NULL) {
            printf("Warning: Frame %d is NULL. Assigning fallback image.\n", i);
            player.chocoFrames[i] = player.chocoFrames[1]; // Replace with your fallback image
        }
    }

    float maxHeight = 0.f;
    for (int i = 0; i < CHOCOSHOOTER_FRAMES; i++) {
        if (player.chocoFrames[i] != NULL) {
            float frameHeight = get_preserved_heightf(
                (float)CP_Image_GetWidth(player.chocoFrames[i]),
                (float)CP_Image_GetHeight(player.chocoFrames[i]),
                player.width);
            if (frameHeight > maxHeight)
                maxHeight = frameHeight;
        }
    }
    player.height = maxHeight; // Ensure uniform height for all frames

    player.hitbox.pos.x = player.pos.x;
    player.hitbox.pos.y = player.pos.y;
    player.hitbox.width = 17.f / 31.f * player.width;
    player.hitbox.height = 233.f / 253.f * player.height / 1.75f;
    player.currentFrame = 0;
    player.frameTime = frameTime; // Use the provided frame time
    player.timeAccumulator = 0.0f;
    player.lastPausedFrame = 0;

    return player;
}

// Originally written by Law Zong Han and Dalton Koh
// Rewritten by Muhammad Nur Fadzly to fix issues and refining.
void move_player(struct ChocoShooter* player, float dt, float level_width, float level_height, const struct Rectangle* clip_entities, const int clip_count) {
    if (_walkSoundTimer < WALK_SOUND_INTERVAL)
        _walkSoundTimer += dt;
    CP_Vector moveDirection = CP_Vector_Zero();

    // Player movement and setting movement
    player->movement = STOPPED;
    if (CP_Input_KeyDown(KEY_W) || CP_Input_KeyDown(KEY_UP))
        player->movement |= MOVING_UP;
    if (CP_Input_KeyDown(KEY_S) || CP_Input_KeyDown(KEY_DOWN))
        player->movement |= MOVING_DOWN;
    if (CP_Input_KeyDown(KEY_A) || CP_Input_KeyDown(KEY_LEFT))
        player->movement |= MOVING_LEFT;
    if (CP_Input_KeyDown(KEY_D) || CP_Input_KeyDown(KEY_RIGHT))
        player->movement |= MOVING_RIGHT;

    // W+S and A+D inputs should stop the character
    // Clear both UP and DOWN bits
    // Clear both LEFT and RIGHT bits
    if ((player->movement & MOVING_UP) && (player->movement & MOVING_DOWN))
        player->movement &= ~(MOVING_UP | MOVING_DOWN);
    if ((player->movement & MOVING_LEFT) && (player->movement & MOVING_RIGHT))
        player->movement &= ~(MOVING_LEFT | MOVING_RIGHT);

    switch (player->movement) {
    case MOVING_UP:
        moveDirection.y -= 1.f;
        break;
    case MOVING_RIGHT:
        moveDirection.x += 1.f;
        break;
    case MOVING_DOWN:
        moveDirection.y += 1.f;
        break;
    case MOVING_LEFT:
        moveDirection.x -= 1.f;
        break;
    case MOVING_UP | MOVING_RIGHT:
        moveDirection.y -= 1.f;
        moveDirection.x += 1.f;
        break;
    case MOVING_UP | MOVING_LEFT:
        moveDirection.y -= 1.f;
        moveDirection.x -= 1.f;
        break;
    case MOVING_DOWN | MOVING_RIGHT:
        moveDirection.y += 1.f;
        moveDirection.x += 1.f;
        break;
    case MOVING_DOWN | MOVING_LEFT:
        moveDirection.y += 1.f;
        moveDirection.x -= 1.f;
    }

    // Normalize
    CP_Vector normalizedVec = CP_Vector_Normalize(moveDirection);
    moveDirection = CP_Vector_Scale(normalizedVec, player->speed * dt);

    // Dash movement with spacebar
    if (CP_Input_KeyDown(KEY_SPACE) && !_isDashing && _dashCooldownTimer >= DASH_COOLDOWN) {
        _isDashing = true;
        _dashCooldownTimer = 0.f;
        play_sound(SOUND_PLAYER_DASH);
        _dashTo = CP_Vector_Add(player->pos, CP_Vector_Scale(normalizedVec, DASH_DISTANCE));
    }

    if (_isDashing) {
        _currentDashTime += DASH_SPEED * dt;
        float t = _currentDashTime / 3.f;

        CP_Vector dashDirection = CP_Vector_Set(
            CP_Math_LerpFloat(player->pos.x, _dashTo.x, t),
            CP_Math_LerpFloat(player->pos.y, _dashTo.y, t));

        moveDirection = CP_Vector_Subtract(dashDirection, player->pos);
        _isDashing = _currentDashTime < 1.f;
        _currentDashTime = _isDashing ? _currentDashTime : 0.f;
    }
    if (!_isDashing && _dashCooldownTimer < DASH_COOLDOWN) // Don't use else if so timer can start right after
        _dashCooldownTimer += dt;

    // Check for any collisions, then update the movement direction
    for (int i = 0; i < clip_count; i++)
        clamp_pos_clip_rel(player->hitbox, &moveDirection, clip_entities[i]);

    player->pos = CP_Vector_Add(player->pos, moveDirection);

    // Use hitbox instead because images can have whitespaces in them
    // Add some padding
    float halfWidth = (player->hitbox.width / 2.f) + 2.f,
        halfHeight = (player->hitbox.height / 2.f) + 2.f;
    player->pos.x = CP_Math_ClampFloat(player->pos.x, halfWidth, level_width - halfWidth);
    player->pos.y = CP_Math_ClampFloat(player->pos.y, halfHeight, level_height - halfHeight);

    // Movement sounds
    if (_walkSoundTimer >= WALK_SOUND_INTERVAL && player->movement != STOPPED) {
        play_sound(SOUND_PLAYER_WALK);
        _walkSoundTimer = 0.f;
    }
}

void render_player(struct ChocoShooter* player, int alpha, float deltaTime) {
    // Update animation frame timing only if the player is moving
    if (player->movement != STOPPED) {
        player->timeAccumulator += deltaTime;
        if (player->timeAccumulator >= player->frameTime) {
            player->timeAccumulator -= player->frameTime;
            player->currentFrame = (player->currentFrame + 1) % CHOCOSHOOTER_FRAMES;
            player->lastPausedFrame = player->currentFrame; // Save the current frame as the last paused frame
        }
    }

    // Render based on movement state
    if (player->movement == STOPPED) {
        // Render the last paused frame
        CP_Image_Draw(player->chocoFrames[player->lastPausedFrame],
            player->pos.x, player->pos.y,
            player->width, player->height, alpha);
    }
    else {
        // Render the current animation frame
        CP_Image_Draw(player->chocoFrames[player->currentFrame],
            player->pos.x, player->pos.y,
            player->width, player->height, alpha);
    }
}
