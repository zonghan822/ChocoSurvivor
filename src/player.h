#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"
#include "flicker.h"

#define PLAYER_HITBOX_OFFSET_X(playerWidth) (- 1 / 33.f * playerWidth)
#define PLAYER_HITBOX_OFFSET_Y(playerHeight) (- 8.f / 77.f * playerHeight)

// to bring over to next level (map)
extern struct ChocoShooter playerInstance;

enum BuffType {
    NO_BUFF,
    ATTACK_UP,
    ATTACK_SPEED_UP,
    HEALTH_UP,
    HEAL,
    ALLYLVL_UP,
    BARRIERLVL_UP
};

struct Buffs {
    enum BuffType type[5];   // Current buff type (can be set to the latest buff type obtained)
    int attackPowerLevel;
    int attackSpeedLevel;
    int healthLevel;
    int allyLevel;
    int barrierLevel;
};

struct ChocoShooter {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
    enum Movement movement;
    int health, maxHealth;
    struct Rectangle hitbox;
    struct FlickerState flickerState;
    struct Buffs buffs;
    int attackPower;   
    float attackSpeed;
    int shield; 
    int activeBuffSlot, activeBuffType, activeBuffLevel;
    int exp;
    int playerMaxExp;
    int playerLevel;
};

#pragma region Player
int player_take_damage(struct ChocoShooter* player, int damage);
struct ChocoShooter init_player(CP_Vector pos, float speed, float maxHealth, int atk, float fireRate);
void move_player(struct ChocoShooter* player, float dt, float level_width, float level_height);
void render_player(struct ChocoShooter player, float alpha);
void render_health_bar(struct ChocoShooter* player, float dt, float windowWidth, float windowHeight, float meterX, float meterY, float meterWidth, float meterHeight);
#pragma endregion

#pragma region Buffs
void apply_buff(struct ChocoShooter* player, enum BuffType buff, struct Barrier* barrier, struct Ally* allies);
int update_buff_meter(struct ChocoShooter* player, int expGained);
void activate_buff_from_slot(struct ChocoShooter* player, int slot);
void assign_buff_to_slot(struct ChocoShooter* player, enum BuffType buffType);
#pragma endregion
