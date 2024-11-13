#include "cprocessing.h"
#include <stdio.h>
#include "player.h"
#include "utils.h"
#include "camera.h"
#include "flicker.h"

#define DASH_COOLDOWN 1.0f
#define DASH_DISTANCE 150.0f
#define DASH_SPEED 20.0f

BOOL _isDashing = FALSE;
float _dashProgress, _lastDash;
CP_Vector _dashTo;


// Returns 1 if still alive, 0 if not
int player_take_damage(struct ChocoShooter* player, int damage) {
    if (player->flickerState.isFlickering) {
        printf("chocoshooter is invincible and cannot take damage right now.\n");
        return 1;  // Skip the rest of the function if flickering
    }

    player->health = CP_Math_ClampInt(player->health - damage, 0, player->maxHealth);

    if (player->health > 0) {
        printf("Player took %d damage, health is now %d\n", damage, player->health);
        flicker(&(player->flickerState), 1);
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

    struct ChocoShooter player = {
        .hitbox.pos = player.pos = pos,
        .speed = speed,
        .health = player.maxHealth = maxHealth,
        .img = CP_Image_Load("Assets/choco_shooter.png"),
        .movement = STOPPED,
        .hitbox.width = player.width = 120.0,
        .hitbox.height = player.height = get_preserved_heightf((float)CP_Image_GetWidth(player.img),
        (float)CP_Image_GetHeight(player.img), player.width),
        .bullets[0].isShooting = FALSE,
        .attackSpeed = fireRate,
        .attackPower = atk,
        .buffs = {0},
        .buffs.maxBuffMeter = 20
    };

    player.hitbox.width /= 1.4f;
    player.hitbox.height /= 1.4f;

    return player;
}

void move_player(struct ChocoShooter *player, float dt) {
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
}

void render_player(struct ChocoShooter player, float alpha, struct Camera cam) {
    CP_Vector vec = get_cam_pos_rel(player.pos, cam);
    CP_Image_Draw(player.img, vec.x, vec.y, player.width, player.height, alpha);
}

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
    bullet->pos.x = (bullet->shootDirection.x < 0) ? player->pos.x - (player->width / 2) : player->pos.x + (player->width / 2);
    bullet->pos.y = player->pos.y;

    int bulletCount = 0;
    for (int i = 0; (player->bullets[i].isShooting || player->bullets[i + 1].isShooting); i++) {
        
        // pop first index if finish shooting
        if (!(player->bullets[i].isShooting) && (player->bullets[i + 1].isShooting)) {
            struct Bullet temp = player->bullets[i];
            player->bullets[i] = player->bullets[i + 1];
            player->bullets[i + 1] = temp;
        }
        bulletCount++;
        if (i + 2 == MAX_BULLET_COUNT) {
            bulletCount = player->bullets[i + 1].isShooting ? bulletCount + 1 : bulletCount;
            break;
        }   
    }
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
        CP_Graphics_DrawCircle(vec.x, vec.y, bullet.diameter);
    } 
}


void apply_buff(struct ChocoShooter* player, enum BuffType buff) {
    switch (buff) {
    case ATTACK_UP:
        player->attackPower += 5;  // Increase player's attack power
        player->buffs.attackP_lvl++;  // Increase the attackP_lvl in playerBuff
        printf("Attack Power Level: %d\n", player->buffs.attackP_lvl);  
        if (player->buffs.attackP_lvl <= 1) {
            assign_buff_to_slot(player, ATTACK_UP);
        }
       
        break;
    case ATTACK_SPEED_UP:
        player->attackSpeed += 1;
        player->buffs.attackS_lvl++;
        printf("Attack Speed Level: %d\n", player->buffs.attackS_lvl);
        if (player->buffs.attackS_lvl <= 1) {
            assign_buff_to_slot(player, ATTACK_SPEED_UP);
        }
        break;
    case HEALTH_UP:
       player->health += 10;
        player->buffs.health_lvl++;
        printf("Health Level: %d\n", player->buffs.health_lvl);
        if (player->buffs.health_lvl <= 1) {
            assign_buff_to_slot(player, HEALTH_UP);
        }
        break;
    }
}

int update_buff_meter(struct ChocoShooter* player, int expGained) {
    // Increase buff experience by the amount gained
    player->buffs.buffExp += expGained;
    printf("Current Buff Exp: %d / %d\n", player->buffs.buffExp, player->buffs.maxBuffMeter);

    // Check if buffExp has reached or exceeded maxBuffMeter
    if (player->buffs.buffExp >= player->buffs.maxBuffMeter) {
        // Wrap around the excess experience
        player->buffs.buffExp -= player->buffs.maxBuffMeter;
        printf("Buff Meter Full! Granting buff...\n");

        // Generate a random buff and return it
        enum BuffType randomBuff = CP_Random_RangeInt(1, 3);
        
        return randomBuff;
    }

    return 0; // Return 0 if no buff is granted
}
#include <stdio.h>  // Ensure this is at the top for printing to work

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
        player->activeBuffLevel = player->buffs.attackP_lvl;
        printf("Active buff set to ATTACK_UP with level %d\n", player->activeBuffLevel);
        break;
    case ATTACK_SPEED_UP:
        player->activeBuffLevel = player->buffs.attackS_lvl;
        printf("Active buff set to ATTACK_SPEED_UP with level %d\n", player->activeBuffLevel);
        break;
    case HEALTH_UP:
        player->activeBuffLevel = player->buffs.health_lvl;
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