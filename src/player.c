#include "cprocessing.h"
#include "player.h"
#include "utils.h"
#include <stdio.h>
#include "camera.h"

#define DASH_COOLDOWN 2.0f
#define DASH_DISTANCE 150.0f
#define DASH_SPEED 10.0f

BOOL _isDashing = FALSE;
float _dashProgress, _lastDash;
CP_Vector _dashTo;

// Returns 1 if still alive, 0 if not
int player_take_damage(struct ChocoShooter* player, int damage) {
    player->health = CP_Math_ClampInt(player->health - damage, 0, player->maxHealth);

    if (player->health > 0) {
        printf("Player took %d damage, health is now %d\n", damage, player->health);
        return 1;
    }

    printf("Choco Shooter has been defeated!\n");
    return 0;
}

struct ChocoShooter init_player(CP_Vector pos, float speed, float maxHealth) {
    _isDashing = FALSE;
    _dashProgress = 0.0;
    _lastDash = 2.1;
    _dashTo = CP_Vector_Zero();

    struct ChocoShooter player = {
        .hitbox.pos = player.pos = pos,
        .speed = speed,
        .health = player.maxHealth = maxHealth,
        .img = CP_Image_Load("Assets/choco_shooter.png"),
        .isFlipped = FALSE,
        .movement = STOPPED,
        .hitbox.width = player.width = 120.0,
        .hitbox.height = player.height = get_preserved_heightf((int)CP_Image_GetWidth(player.img),
        (int)CP_Image_GetHeight(player.img), player.width)
    };

    player.hitbox.width /= 1.4f;
    player.hitbox.height /= 1.4f;

    return player;
}

void move_player(struct ChocoShooter *player, float dt, float level_width, float level_height) {
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
    if (CP_Input_KeyTriggered(KEY_SPACE) && !_isDashing && _lastDash >= DASH_COOLDOWN) {
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

        if (_dashProgress >= 1.0f)
            _isDashing = FALSE;
    }

    // Use hitbox instead because images can have whitespaces in them
    // Add some padding
    float halfWidth = (player->hitbox.width / 2.0f) + 5.0f,
        halfHeight = (player->hitbox.height / 2.0f) + 5.0f;

    player->pos.x = CP_Math_ClampFloat(player->pos.x, halfWidth, level_width - halfWidth);
    player->pos.y = CP_Math_ClampFloat(player->pos.y, halfHeight, level_height - halfHeight);
}

void render_player(struct ChocoShooter player, float alpha, struct Camera cam) {
    CP_Vector vec = get_cam_pos_rel(player.pos, cam);

    CP_Image_Draw(player.img, vec.x, vec.y, player.width, player.height, alpha);
}