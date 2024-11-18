#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "enemy.h"
#include "camera.h"

struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float spawnWidth, float xSpeed, float speed, int maxHealth, int dmg) {
    srand((unsigned int)time(NULL));
    struct ChocoRoach chocoRoach = {
        .img = CP_Image_Load("Assets/choco_roach.png"),
        .width = 70.f,
        .height = get_preserved_heightf(
            (float)CP_Image_GetWidth(chocoRoach.img),
            (float)CP_Image_GetHeight(chocoRoach.img),
            chocoRoach.width
        ),
        .pos.x = CP_Random_RangeFloat(-chocoRoach.width - spawnWidth, -chocoRoach.width),
        .pos.y = CP_Random_RangeFloat(spawnMinY, spawnMaxY),
        .speed = xSpeed,                // Set the current speed
        .originalSpeed = xSpeed,        // Store the original speed
        .health = maxHealth,
        .maxHealth = maxHealth,
        .dmg = dmg,
        .moveDirection = CP_Vector_Zero(), 
        .currentState = STATE_IDLE,
        .hitbox.pos.x = 0.0f,           // Initialize relative hitbox positions
        .hitbox.pos.y = 0.0f,
        .hitbox.width = 0.8f * chocoRoach.width,
        .hitbox.height = 0.75f * chocoRoach.height,
    };
    chocoRoach.hitbox.pos.x = chocoRoach.pos.x + ENEMY_HITBOX_OFFSET_X(chocoRoach.width);
    chocoRoach.hitbox.pos.y = chocoRoach.pos.y + ENEMY_HITBOX_OFFSET_Y(chocoRoach.height);
    return chocoRoach;
}

void enemy_move_to_target(struct ChocoRoach* roach, struct Rectangle hitbox, float dt) {
    roach->moveDirection = CP_Vector_Normalize(CP_Vector_Subtract(hitbox.pos, roach->pos));
    roach->pos.x += roach->moveDirection.x * roach->speed * dt;
    roach->pos.y += roach->moveDirection.y * roach->speed * dt;
}

void render_enemy_health_bar(struct ChocoRoach* roach, struct Camera cam) {
    CP_Vector vec = get_cam_pos_rel(roach->pos, cam);

    float health_bar_x = vec.x;
    float health_bar_y = vec.y - (roach->height / 2) - 10.f;
    float health_bar_w = roach->width * 0.8f;
    float health_bar_h = 15.f;

    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    CP_Graphics_DrawRect(health_bar_x, health_bar_y, health_bar_w, health_bar_h);

    float health_gap_x = health_bar_x + ((float)roach->health / (float)roach->maxHealth) * health_bar_w;
    float health_gap_w = health_bar_w * (1.f - ((float)roach->health / (float)roach->maxHealth));

    CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));
    CP_Graphics_DrawRect(health_gap_x, health_bar_y, health_gap_w, health_bar_h);
}

void render_enemies(struct ChocoRoach* roaches, struct Camera cam, int size) {
    for (int i = 0; i < size; i++) {
        if (roaches[i].currentState != STATE_DIED) {
            CP_Vector vec = get_cam_pos_rel(roaches[i].pos, cam);
            CP_Image_Draw(roaches[i].img, vec.x, vec.y, roaches[i].width, roaches[i].height, 255);
        }
    }
}

int _comp(const void* a, const void* b) {
    const struct ChocoRoach* ra = (const struct ChocoRoach*)a;
    const struct ChocoRoach* rb = (const struct ChocoRoach*)b;
    if (!ra->currentState && !rb->currentState) return -1;
    if (!ra->currentState) return 1;
    if (!rb->currentState) return -1;
    return (ra->pos.y == rb->pos.y) ? (ra->pos.x - rb->pos.x) : (ra->pos.y - rb->pos.y);
}

void sort_enemies(struct ChocoRoach* roaches, int size) {
    qsort(roaches, size, sizeof(struct ChocoRoach), _comp);
}

void enemy_take_damage(struct ChocoRoach* roach, int dmg) {
    roach->health = CP_Math_ClampInt(roach->health - dmg, 0, roach->maxHealth);
    roach->isTakingDamage = TRUE;
    if (roach->health <= 0) {
        roach->currentState = STATE_DIED;
    }
}
