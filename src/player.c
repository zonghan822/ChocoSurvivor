#include "cprocessing.h"
#include <stdio.h>
#include "player.h"
#include "utils.h"
#include "camera.h"
#include "flicker.h"

static const float DASH_COOLDOWN = 1.f,
    DASH_DISTANCE = 150.f, DASH_SPEED = 20.f;

static const float EXP_INCREASE_MULTIPLIER = 1.35f;

static BOOL _isDashing = FALSE;
static float _dashProgress, _lastDash;
static CP_Vector _dashTo;

int playerMaxExp = 20;
int playerLevel = 1;
struct ChocoShooter playerInstance;

#pragma region Player
// Returns 1 if still alive, 0 if not
int player_take_damage(struct ChocoShooter* player, int damage) {
    if (player->flickerState.isFlickering) {
        printf("chocoshooter is invincible and cannot take damage right now.\n");
        return 1;  // Skip the rest of the function if flickering
    }

    flicker(&(player->flickerState), 1);
    player->health = CP_Math_ClampInt(player->health - damage, 0, player->maxHealth);

    if (player->health > 0) {
        printf("Player took %d damage, health is now %d\n", damage, player->health);
        return 1;
    }

    printf("Choco Shooter has been defeated!\n");
    return 0;
}

void init_player(CP_Vector pos, float speed, float maxHealth, int atk, float fireRate) {
    _isDashing = FALSE;
    _dashProgress = 0.0;
    _lastDash = 2.1;
    _dashTo = CP_Vector_Zero();

    struct ChocoShooter player = {
        .pos = pos,
        .hitbox.pos = pos,
        .speed = speed,
        .health = maxHealth,
        .maxHealth = maxHealth,
        .img = CP_Image_Load("Assets/choco_shooter.png"),
        .movement = STOPPED,
        .hitbox.width = player.width = 100.f,
        .hitbox.height = player.height = get_preserved_heightf((float)CP_Image_GetWidth(player.img),
            (float)CP_Image_GetHeight(player.img), player.width),
        .bullets[0].isShooting = FALSE,
        .attackSpeed = fireRate,
        .attackPower = atk,
        .buffs = {0},
        .flickerState.isVisible = TRUE,
        .exp = 0
    };
    player.height = get_preserved_heightf((float)CP_Image_GetWidth(player.img), (float)CP_Image_GetHeight(player.img), player.width);
    player.hitbox.width = player.width / 1.4f;
    player.hitbox.height = player.height / 1.4f;

    playerInstance = player;
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

    // Use hitbox instead because images can have whitespaces in them
    // Add some padding
    float halfWidth = (player->hitbox.width / 2.0f) + 2.0f,
        halfHeight = (player->hitbox.height / 2.0f) + 2.0f;

    player->pos.x = CP_Math_ClampFloat(player->pos.x, halfWidth, level_width - halfWidth);
    player->pos.y = CP_Math_ClampFloat(player->pos.y, halfHeight, level_height - halfHeight);
}

void render_player(struct ChocoShooter player, float alpha, struct Camera cam) {
    CP_Vector vec = get_cam_pos_rel(player.pos, cam);
    CP_Image_Draw(player.img, vec.x, vec.y, player.width, player.height, alpha);
}

#pragma endregion

#pragma region Bullet
struct Bullet init_bullet(enum BulletType bulletType, float bulletSpeed, float bulletDiameter, int dmg, CP_Vector bulletDirection) {
    struct Bullet bullet = {
        .isShooting = TRUE,
        .shootRange = 1000.f,
        .bulletType = bulletType,
        .speed = bulletSpeed,
        .diameter = bulletDiameter,
        .dmg = dmg,
        .shootDirection = bulletDirection
    };
    return bullet;
}

void shoot_bullet(struct ChocoShooter* player, struct Bullet* bullet) {
    // x is the min or max x of player (extreme left or right of player)
    float playerWidth = player->width < 0 ? -player->width : player->width;
    bullet->pos.x = (bullet->shootDirection.x < 0) ? player->pos.x - (playerWidth / 2) : player->pos.x + (playerWidth / 2);
    bullet->pos.y = player->pos.y;

    int bulletCount = 0;
    // check if the bullet is still moving
    for (int i = 0; (player->bullets[i].isShooting || player->bullets[i + 1].isShooting); i++) {

        // pop first index if finish shooting (push to the back of the last active bullet)
        // e.g.: 
        // before: [b1, b2, b3, b4, b5]->.ishooting = [False, True, True, True, False]
        // after: [b2, b3, b4, b1, b5]->.ishooting = [True, True, True, False, False] , bulletCount = 3
    
        // if first bullet finish shooting and second bullet still shooting
        if (!(player->bullets[i].isShooting) && (player->bullets[i + 1].isShooting)) {
            // swap bullet
            struct Bullet temp = player->bullets[i];
            player->bullets[i] = player->bullets[i + 1];
            player->bullets[i + 1] = temp;
        }

        // get the count of the active bullet
        bulletCount++;
        if (i + 2 == MAX_BULLET_COUNT) {
            bulletCount = player->bullets[i + 1].isShooting ? bulletCount + 1 : bulletCount;
            break;
        }
    }

    // assign the next bullet to the next active bullet slot 
    if (bulletCount < MAX_BULLET_COUNT) {
        player->bullets[bulletCount] = *bullet;
    }
}

void update_pos_bullet(struct Bullet* bullet, float dt) {

    if (bullet->isShooting) {
        if (bullet->shootRange > 0) {
            bullet->shootDirection = CP_Vector_Normalize(bullet->shootDirection);
            bullet->pos.x += bullet->shootDirection.x * bullet->speed * dt;
            bullet->pos.y += bullet->shootDirection.y * bullet->speed * dt;
            bullet->shootRange -= bullet->speed * dt;
        }
        else {
            bullet->isShooting = FALSE;
        }
    }
}

void render_bullet(struct Bullet bullet, struct Camera cam) {
    if (bullet.isShooting) {
        CP_Vector vec = get_cam_pos_rel(bullet.pos, cam);
        CP_Settings_Fill(CP_Color_Create(113, 54, 0, 128));
        CP_Graphics_DrawCircle(vec.x, vec.y, bullet.diameter);
    }
}

#pragma endregion

#pragma region Buff
void apply_buff(struct ChocoShooter* player, enum BuffType buff) {
    switch (buff) {
    case ATTACK_UP:
        player->attackPower += 5;  // Increase player's attack power
        player->buffs.attackPowerLevel++;  // Increase the attackPowerLevel in playerBuff
        printf("Attack Power Level: %d\n", player->buffs.attackPowerLevel);
        if (player->buffs.attackPowerLevel <= 1) {
            assign_buff_to_slot(player, ATTACK_UP);
        }
       
        break;
    case ATTACK_SPEED_UP:
        player->attackSpeed += 1;
        player->buffs.attackSpeedLevel++;
        printf("Attack Speed Level: %d\n", player->buffs.attackSpeedLevel);
        if (player->buffs.attackSpeedLevel <= 1) {
            assign_buff_to_slot(player, ATTACK_SPEED_UP);
        }
        break;
    case HEALTH_UP:
       player->health += 10;
        player->buffs.healthLevel++;
        printf("Health Level: %d\n", player->buffs.healthLevel);
        if (player->buffs.healthLevel <= 1) {
            assign_buff_to_slot(player, HEALTH_UP);
        }
        break;
    }
}

// Sidenote: EXP would refer to buff meter
int update_buff_meter(struct ChocoShooter* player, int expGained) {
    player->exp += expGained;
    printf("Current Exp: %d / %d\n", player->exp, playerMaxExp);

    // Full or exceeded EXP
    if (player->exp >= playerMaxExp) {
        playerLevel++;
        // Do nothing if EXP == playerMaxExp
        // But do return the buff
        // Wrap around only if exceeding
        if (player->exp > playerMaxExp) {
            // Wrap around then increase maxExp
            player->exp -= playerMaxExp;
            playerMaxExp *= EXP_INCREASE_MULTIPLIER;
        }
        enum BuffType randomBuff = CP_Random_RangeInt(1, 3);
        return randomBuff;
    }

    return 0; // Return 0 if no buff is granted
}

void activate_buff_from_slot(struct ChocoShooter* player, int slot) {
    //debug
    if (player->buffs.type[slot - 1] == NO_BUFF) {
        printf("Slot %d is empty, cannot activate buff.\n", slot);
        return;
    }

    // Capture the buff type and level from the selected slot before clearing it
    player->activeBuffType = player->buffs.type[slot - 1];

    switch (player->activeBuffType) {
    case ATTACK_UP:
        player->activeBuffLevel = player->buffs.attackPowerLevel;
        printf("Active buff set to ATTACK_UP with level %d\n", player->activeBuffLevel);
        break;
    case ATTACK_SPEED_UP:
        player->activeBuffLevel = player->buffs.attackSpeedLevel;
        printf("Active buff set to ATTACK_SPEED_UP with level %d\n", player->activeBuffLevel);
        break;
    case HEALTH_UP:
        player->activeBuffLevel = player->buffs.healthLevel;
        printf("Active buff set to HEALTH_UP with level %d\n", player->activeBuffLevel);
        break;
    default:
        player->activeBuffLevel = 0;  
        printf("No valid buff in slot %d.\n", slot);
        return;
    }

    // Set the slot visually to empty
    player->buffs.type[slot - 1] = NO_BUFF;
    printf("Cleared buff type from slot %d.\n", slot);  // Debug 
}

void assign_buff_to_slot(struct ChocoShooter* player, enum BuffType buffType) {
    for (int i = 0; i < 3; i++) {
        if (player->buffs.type[i] == NO_BUFF) {  // Find the first empty slot
            player->buffs.type[i] = buffType;
            break;
        }
    }
}

#pragma endregion
