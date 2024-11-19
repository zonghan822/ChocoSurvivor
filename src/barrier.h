#pragma once
#define MAX_BARRIERS 10
#include "cprocessing.h"
#include "player.h"
#define BARRIER_INITIAL_HEALTH 5
#define MAX_ABULLET_COUNT 20
#define MAX_ALLIES 3
#define ALLY_BULLET_SPEED 800.0f

struct Barrier {
    CP_Vector position;
    float width;
    float height;
    int health;
    float cooldownTimer;
};

// Enum for barrier states
enum BarrierState {
    BARRIER_IDLE,
    BARRIER_PREVIEW
};

// Functions for managing barriers
void takeDamage(int damage);
int get_barrier_count(void);
void increment_barrier_count(void);
void set_barrier_properties(int index, CP_Vector position, float width, float height, int health);
struct Barrier* get_barriers(void);  // Add this line
void check_barrier_collision(struct ChocoRoach* roach);
void reset_barriers(void);

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
};

// Functions for managing allies
void init_ally_properties(int index, CP_Vector position);
void update_allies(struct Ally* allies, int allyCount, struct ChocoRoach* enemies, int enemyCount, float dt);
void render_allies(struct Ally* allies, int allyCount, struct Camera cam);
void shoot_ally_bullet(struct Ally* ally, struct Bullet* bullet, struct ChocoRoach* target);
void add_ally(CP_Vector position);
struct Ally* get_allies(void);
int get_ally_count(void);