#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"

#define MAX_BARRIERS 10
#define BARRIER_INITIAL_HEALTH 5
#define MAX_ABULLET_COUNT 20
#define MAX_ALLIES 3
#define ALLY_BULLET_SPEED 800.0f

struct Barrier {
    CP_Vector position;
    float width;
    float height;
    int health;
    struct Rectangle hitbox;
    float cooldownTimer;
};

enum BarrierState {
    BARRIER_IDLE,
    BARRIER_PREVIEW
};

#pragma region Barrier
// Functions for managing barriers
void barrier_take_damage(int damage);
int get_barrier_count(void);
CP_Image get_barrier_image(int barrierLevel);
void increment_barrier_count(void);
void set_barrier_properties(int index, CP_Vector position, float width, float height, int health);
struct Barrier* get_barriers(void);  // Add this line
void check_barrier_collision(struct ChocoRoach* roach, float dt);
void reset_barriers(void);
#pragma endregion

enum AllyState {
    ALLY_IDLE,
    ALLY_PREVIEW,
};

struct Ally {
    CP_Vector position;
    float width;
    float height;
    int health;
    int maxHealth;
    float attackSpeed;
    int attackPower;
    struct Bullet bullets[MAX_ABULLET_COUNT];
    float lastShotTime;
    CP_Image image;  // Ally's image
    enum AllyState state;  
};

#pragma region Allies
// Functions for managing allies
void init_ally_properties(int index, CP_Vector position, float width, float height, int health);
void update_allies(float dt, struct ChocoRoach* enemies, int enemyCount, struct Camera cam);
struct Ally* get_allies(void);
int get_ally_count(void);
int find_bullet_slot(struct Bullet* bullets);
void reset_allies(void);
void increment_ally_count(void);
CP_Image get_ally_image(void);
void render_allies(struct Camera cam);
void render_abullet(struct Bullet bullet, struct Camera cam);
#pragma endregion
