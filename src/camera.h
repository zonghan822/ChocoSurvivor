#pragma once
#include "cprocessing.h"

void init_camera(struct ChocoShooter player, float width, float height);

void update_camera(struct ChocoShooter player, float dt);

void apply_camera(void);