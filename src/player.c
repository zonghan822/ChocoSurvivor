#include "cprocessing.h"
#include "player.h"
#include "utils.h"
#include "camera.h"
#include "flicker.h"
#include "level1.h"
#include "placeables.h"
#include "audio.h"

#include <stdio.h>

static const float DASH_COOLDOWN = 1.f,
    DASH_DISTANCE = 150.f, DASH_SPEED = 20.f;
static const float WALK_SOUND_INTERVAL = 0.5f;

static BOOL _isDashing = FALSE;
static float _dashProgress, _lastDash;
static CP_Vector _dashTo;
static float _walkSoundTimer;

struct ChocoShooter playerInstance; // Global variable, not private

// Returns 1 if still alive, 0 if not
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

struct ChocoShooter init_player(CP_Vector pos, float speed, int maxHealth, int atk, float fireRate) {
    _isDashing = false;
    _dashProgress = 0.0;
    _lastDash = 2.1f;
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
    player.height = get_preserved_heightf((float)CP_Image_GetWidth(player.img), (float)CP_Image_GetHeight(player.img), player.width);
    player.hitbox.pos.x = player.pos.x + PLAYER_HITBOX_OFFSET_X(player.width);
    player.hitbox.pos.y = player.pos.y + PLAYER_HITBOX_OFFSET_Y(player.height);
    player.hitbox.width = 17.f / 31.f * player.width;
    player.hitbox.height = 233.f / 253.f * player.height;

    return player;
}

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
        moveDirection.y -= 1.0f;
        break;
    case MOVING_RIGHT:
        moveDirection.x += 1.0f;
        break;
    case MOVING_DOWN:
        moveDirection.y += 1.0f;
        break;
    case MOVING_LEFT:
        moveDirection.x -= 1.0f;
        break;
    case MOVING_UP | MOVING_RIGHT:
        moveDirection.y -= 1.0f;
        moveDirection.x += 1.0f;
        break;
    case MOVING_UP | MOVING_LEFT:
        moveDirection.y -= 1.0f;
        moveDirection.x -= 1.0f;
        break;
    case MOVING_DOWN | MOVING_RIGHT:
        moveDirection.y += 1.0f;
        moveDirection.x += 1.0f;
        break;
    case MOVING_DOWN | MOVING_LEFT:
        moveDirection.y += 1.0f;
        moveDirection.x -= 1.0f;
    }

    // Normalize
    CP_Vector normalizedVec = CP_Vector_Zero();
    if (moveDirection.x != 0 || moveDirection.y != 0)
        normalizedVec = CP_Vector_Normalize(moveDirection);

    moveDirection.x = normalizedVec.x * player->speed * dt;
    moveDirection.y = normalizedVec.y * player->speed * dt;

    // Check for any collisions, then update the movement direction
    for (int i = 0; i < clip_count; i++)
        clamp_pos_clip_rel(player->hitbox, &moveDirection, clip_entities[i]);

    player->pos.x += moveDirection.x;
    player->pos.y += moveDirection.y;

    if (_lastDash < DASH_COOLDOWN)
        _lastDash += dt;

    // Dash movement with spacebar
    if (CP_Input_KeyDown(KEY_SPACE) && !_isDashing && _lastDash >= DASH_COOLDOWN) {
        play_sound(SOUND_PLAYER_DASH);
        _dashTo = player->pos;
        _isDashing = true;
        _dashProgress = 0.0;
        _lastDash = 0.0;

        if (normalizedVec.x != 0 || normalizedVec.y != 0) {
            _dashTo.x += normalizedVec.x * DASH_DISTANCE;
            _dashTo.y += normalizedVec.y * DASH_DISTANCE;
        }
    }

    if (_isDashing) {
        _dashProgress += (DASH_SPEED * dt) / 3.f;

        player->pos.x = CP_Math_LerpFloat(player->pos.x, _dashTo.x, _dashProgress);
        player->pos.y = CP_Math_LerpFloat(player->pos.y, _dashTo.y, _dashProgress);

        _isDashing = _dashProgress < 1.f;
    }

    // Use hitbox instead because images can have whitespaces in them
    // Add some padding
    float halfWidth = (player->hitbox.width / 2.0f) + 2.0f,
        halfHeight = (player->hitbox.height / 2.0f) + 2.0f;

    player->pos.x = CP_Math_ClampFloat(player->pos.x, halfWidth, level_width - halfWidth);
    player->pos.y = CP_Math_ClampFloat(player->pos.y, halfHeight, level_height - halfHeight);

    // Movement sounds
    if (_walkSoundTimer >= WALK_SOUND_INTERVAL && player->movement != STOPPED) {
        play_sound(SOUND_PLAYER_WALK);
        _walkSoundTimer = 0.f;
    }
}

void render_player(struct ChocoShooter player, int alpha) {
    CP_Image_Draw(player.img, player.pos.x, player.pos.y, player.width, player.height, alpha);
}