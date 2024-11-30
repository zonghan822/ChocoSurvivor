/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
*
* @filename		utils.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli, Law Zong Han
* @DP emails	muhammadnurfadzly.b@digipen.edu, zonghan.law@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This header declares utility functions and structures for
				handling 2D movement and rendering. It includes bit-shift-based
				movement states (e.g., moving up, down, left, right), and defines
				Rectangle and Circle structures. It also provides a health bar 
				renderer that adjusts based on the entity's health, and a custom 
				cursor renderer that scales and positions a given cursor image. 
				Additionally, it includes a helper function to round a dimension 
				up to the nearest multiple of a divisor (mainly for Nine-Patch 
				Rect or 9 Slicing). A function to remove an element from an int 
				array is also included in this file.
******************************************************************************/

#pragma once
#include "cprocessing.h"
#include <stdbool.h>

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))

/********************************
   Made by Muhammad Nur Fadzly
*********************************/
// Use bit-shift
enum Movement {
	STOPPED = 0,
	MOVING_UP = 1 << 0,		// 0b_00000001 // 1	
	MOVING_RIGHT = 1 << 1,	// 0b_00000010 // 2
	MOVING_DOWN = 1 << 2,	// 0b_00000100 // 4
	MOVING_LEFT = 1 << 3	// 0b_00001000 // 8
};

struct Rectangle {
	CP_Vector pos;
	float width, height;
	//CP_POSITION_MODE mode;
};

struct Circle {
	CP_Vector pos;
	float diameter;
};


/********************************
   Made by Muhammad Nur Fadzly
*********************************/
float get_preserved_widthf(float width, float height, float new_height);

float get_preserved_heightf(float width, float height, float new_width);

void clamp_pos_clip_rel(struct Rectangle entity_hitbox, CP_Vector* movement, struct Rectangle clip_hitbox);

float resize_by_divisor(float dimension, int divisor);

void render_cursor(CP_Image cursor);


/*****************************
     Made by Law Zong Han    
******************************/
void render_health_bar(struct Rectangle entity, int health, int maxHealth);

void pop_int_array(int* arr, int *size, int element);
