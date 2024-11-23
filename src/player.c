#include "cprocessing.h"
#include <stdio.h>
#include "player.h"
#include "utils.h"
#include "camera.h"
#include "flicker.h"
#include "level1.h"
#include "placeables.h"
#include "audio.h"

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

    if (player->health > 0) {
        printf("Player took %d damage, health is now %d\n", damage, player->health);
        play_sound(SOUND_PLAYER_HURT);
        return 1;
    }

    printf("Choco Shooter has been defeated!\n");
    return 0;
}

struct ChocoShooter init_player(CP_Vector pos, float speed, float maxHealth, int atk, float fireRate) {
    _isDashing = FALSE;
    _dashProgress = 0.0;
    _lastDash = 2.1;
    _dashTo = CP_Vector_Zero();
    _walkSoundTimer = WALK_SOUND_INTERVAL;

    struct ChocoShooter player = {
        .pos = pos,
        .hitbox.pos = pos,
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
        .expGained = 0
    };
    player.height = get_preserved_heightf((float)CP_Image_GetWidth(player.img), (float)CP_Image_GetHeight(player.img), player.width);
    player.hitbox.width = player.width / 1.4f;
    player.hitbox.height = player.height / 1.4f;

    return player;
}

void move_player(struct ChocoShooter *player, float dt, float level_width, float level_height) {
    if (_walkSoundTimer < WALK_SOUND_INTERVAL)
        _walkSoundTimer += dt;
    CP_Vector moveDirection = CP_Vector_Zero();

    // Player movement and setting movement
    player->movement = STOPPED;
    if (CP_Input_KeyDown(KEY_W))
        player->movement |= MOVING_UP;
    if (CP_Input_KeyDown(KEY_S))
        player->movement |= MOVING_DOWN;
    if (CP_Input_KeyDown(KEY_A))
        player->movement |= MOVING_LEFT;
    if (CP_Input_KeyDown(KEY_D))
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
    if (moveDirection.x != 0 || moveDirection.y != 0)
        moveDirection = CP_Vector_Normalize(moveDirection);

    player->pos.x += moveDirection.x * player->speed * dt;
    player->pos.y += moveDirection.y * player->speed * dt;

    if (_lastDash < DASH_COOLDOWN)
        _lastDash += dt;

    // Dash movement with spacebar
    if (CP_Input_KeyDown(KEY_SPACE) && !_isDashing && _lastDash >= DASH_COOLDOWN) {
        _dashTo = player->pos;
        _isDashing = TRUE;
        _dashProgress = 0.0;
        _lastDash = 0.0;

        if (moveDirection.x != 0 || moveDirection.y != 0) {
            _dashTo.x += moveDirection.x * DASH_DISTANCE;
            _dashTo.y += moveDirection.y * DASH_DISTANCE;
        }
    }

    if (_isDashing) {
        _dashProgress += DASH_SPEED * dt;

        player->pos.x = CP_Math_LerpFloat(player->pos.x, _dashTo.x, _dashProgress);
        player->pos.y = CP_Math_LerpFloat(player->pos.y, _dashTo.y, _dashProgress);

        if (_dashProgress >= 1.5f)
            _isDashing = FALSE;
    }

    update_camera(*player, dt);

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

void render_player(struct ChocoShooter player, float alpha) {
    CP_Image_Draw(player.img, player.pos.x, player.pos.y, player.width, player.height, alpha);
}

void render_health_bar(struct ChocoShooter* player, float dt, float windowWidth, float windowHeight, float meterX, float meterY, float meterWidth, float meterHeight) {
    // Health bar properties

    float barWidth = meterWidth;      // Match the buff bar width
    float barHeight = 10.0f;          // Slightly smaller than the buff bar
    float barOffsetY = meterY - 30.0f; // Position slightly above the buff bar
    static float interpolatedHealthWidth = 0.0f; // Smooth interpolation width

    // Calculate target width based on current health percentage
    float targetHealthWidth = (player->health / (float)player->maxHealth) * barWidth;

    // Interpolate the width towards the target
    if (interpolatedHealthWidth != targetHealthWidth) {
        float delta = targetHealthWidth - interpolatedHealthWidth;
        interpolatedHealthWidth += delta * 10.0f * dt; // Adjust speed
        interpolatedHealthWidth = CP_Math_ClampFloat(interpolatedHealthWidth, 0, barWidth);
    }

    CP_Settings_RectMode(CP_POSITION_CORNER);

    // Render the background of the health bar (gray)
    CP_Settings_Fill(CP_Color_Create(50, 50, 50, 255));
    CP_Graphics_DrawRect(meterX, barOffsetY, barWidth, meterHeight);

    // Render the interpolated health bar (green to red gradient)
    CP_Settings_Fill(CP_Color_Create(225, 0, 0, 255));
    CP_Graphics_DrawRect(meterX, barOffsetY, interpolatedHealthWidth, meterHeight);
}