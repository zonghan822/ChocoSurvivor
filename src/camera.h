/******************************************************************************
* @filename		camera.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       These functions manage the topdown 2D camera.
******************************************************************************/

#pragma once
#include "cprocessing.h"

void init_camera(struct ChocoShooter player, float width, float height);

void update_camera(struct ChocoShooter player, float dt);

void apply_camera(void);