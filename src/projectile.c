/******************************************************************************
* @filename		projectile.c
* @author(s)	Law Zong Han, Muhammad Nur Fadzly
* @DP emails	zonghan.law@digipen.edu, muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file is for creating projectiles by player or an ally.
                Could also be used for enemies but feature is scrapped.
                Functions handle creating, rendering, updating and removing
                projectiles when necessary.

                Basis written by Law Zong Han but restructured/rewritten by
                Muhammad Nur Fadzly.
******************************************************************************/

#include "projectile.h"
#include "player.h"
#include "placeables.h"
#include "enemy.h"
#include "audio.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>


ProjectilePool playerProjectiles;
static float _playerProjectileSpeed = 1000.f, _playerProjectileDiameter = 14.f;
static float _playerLastShotTime = 0.f;

ProjectilePool allyProjectiles[MAX_ALLIES];
static float _allyProjectileSpeed = 750.f, _allyProjectileDiameter = 11.f;
static float _allyLastShotTime[MAX_ALLIES] = { 0 };

static bool _add_projectile(ProjectilePool* pool, struct Projectile projectile) {
    if (pool->activeCount < MAX_PLAYER_PROJECTILES) {
        pool->projectiles[pool->activeCount++] = projectile;
        return true;
    }
    return false;
}

struct Projectile init_projectile(enum ProjectileType projectileType, float projectileSpeed, float diameter, int dmg, CP_Vector direction) {
    struct Projectile projectile = {
        .isActive = true,
        .range = 1000.f,
        .type = projectileType,
        .speed = projectileSpeed,
        .diameter = diameter,
        .dmg = dmg,
        .shootDirection = CP_Vector_Normalize(direction)
    };
    return projectile;
}

void player_shoot(float dt, bool can_shoot, float mouse_x, float mouse_y) {
    _playerLastShotTime += dt;
    if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT) && can_shoot) {
        float shootSpd = (playerInstance.currentProjectileType & FAST_PROJECTILE) ? playerInstance.attackSpeed * 2.f: playerInstance.attackSpeed;
        if (_playerLastShotTime >= 1/shootSpd) { // control fire rate
            play_sound(SOUND_PLAYER_SHOOT);
            // projectile direction is the vector of player with mouse pointer
            CP_Vector projectileDirection = CP_Vector_Subtract(CP_Vector_Set(mouse_x, mouse_y), playerInstance.pos);
            struct Projectile projectile = init_projectile(playerInstance.currentProjectileType, _playerProjectileSpeed,
                _playerProjectileDiameter, playerInstance.attackPower, projectileDirection);

            // Shoot projectile logic
            {
                // ****** Disabled for gameplay adjustment ****** //
                // x is the min or max x of player (extreme left or right of player)
                // float playerWidth = fabsf(playerInstance.width);//playerInstance.width < 0 ? -playerInstance.width : playerInstance.width;
                //projectile.pos.x = (projectile.shootDirection.x < 0)
                //    ? playerInstance.pos.x - (playerWidth / 2) 
                //    : playerInstance.pos.x + (playerWidth / 2);

                projectile.pos.x = playerInstance.pos.x;
                projectile.pos.y = playerInstance.pos.y;

                _add_projectile(&playerProjectiles, projectile);
            }
            _playerLastShotTime = 0.f;
        }
    }
}

// Made by Dalton Koh
void ally_shoot(float dt, struct ChocoRoach* enemies, int enemyCount) {
    for (int i = 0; i < allyCount; i++) {
        if (allies[i].state < ALLY_IDLE)
            continue;

        _allyLastShotTime[i] += dt;

        // Find the closest enemy within 500 pixels
        struct ChocoRoach* closestEnemy = NULL;
        float closestDistance = 500.f;

        for (int j = 0; j < enemyCount; j++) {
            if (enemies[j].currentState == ENEMY_STATE_DIED)
                continue;

            float distance = CP_Vector_Distance(allies[i].pos, enemies[j].pos);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = &enemies[j];
            }
        }

        if (closestEnemy == NULL)
            continue;

        CP_Vector dst = CP_Vector_Subtract(closestEnemy->pos, allies[i].pos);
        allies[i].width = (dst.x > 0 && allies[i].width > 0) || (dst.x <= 0 && allies[i].width < 0)
            ? -allies[i].width 
            : allies[i].width;

        // Shoot if ready and target found
        if (_allyLastShotTime[i] >= (1 / allies[i].attackSpeed)) {
            play_sound(SOUND_PLAYER_SHOOT);
            CP_Vector direction = CP_Vector_Subtract(closestEnemy->pos, allies[i].pos);
            struct Projectile projectile = init_projectile(NORMAL_PROJECTILE,
                _allyProjectileSpeed, _allyProjectileDiameter,
                allies[i].attackPower, direction);

            // Set projectile position to ally's position
            projectile.pos = allies[i].pos;

            // Add projectile to the ally's projectile pool
            if (_add_projectile(&allyProjectiles[i], projectile)) {
                _allyLastShotTime[i] = 0.0f;
            }
        }
    }
}

void remove_projectile(ProjectilePool* pool, int index) {
    if (pool->projectiles[index].isActive)
        pool->projectiles[index].isActive = false;
    struct Projectile temp = pool->projectiles[index];

    // Throw the not active projectile to the back of the last active projectile
    for (int i = index; i < pool->activeCount; i++)
        pool->projectiles[i] = pool->projectiles[i + 1];

    if (pool->projectiles[index].isActive)
        pool->projectiles[--pool->activeCount] = temp;
    else
        pool->projectiles[pool->activeCount] = temp;
}

void update_projectiles(ProjectilePool* pool, float dt) {
    for (int i = 0; i < pool->activeCount; i++) {
        struct Projectile* proj = &pool->projectiles[i];
        if (proj->isActive) {       
            proj->pos.x += proj->shootDirection.x * proj->speed * dt;
            proj->pos.y += proj->shootDirection.y * proj->speed * dt;
            proj->range -= proj->speed * dt;
            if (proj->range <= 0) {
                proj->isActive = false;
                pool->projectiles[i] = pool->projectiles[--pool->activeCount]; // --pool->activeCount is decrement
            }
        }
        else {
            pool->projectiles[i] = pool->projectiles[--pool->activeCount];
        }
    }
}

void update_all_projectiles(float dt) {
    update_projectiles(&playerProjectiles, dt);
    for (int i = 0; i < allyCount; i++) {
        update_projectiles(&allyProjectiles[i], dt);
    }
}

void render_all_projectiles() {
    render_projectiles(&playerProjectiles);
    for (int i = 0; i < allyCount; i++) {
        render_projectiles(&allyProjectiles[i]);
    }
}

void render_projectiles(const ProjectilePool* pool) {
    for (int i = 0; i < pool->activeCount; i++) {
        struct Projectile proj = pool->projectiles[i];
        CP_Settings_Fill(CP_Color_Create(113, 54, 0, 128));
        CP_Graphics_DrawCircle(proj.pos.x, proj.pos.y, proj.diameter);
    }
}
