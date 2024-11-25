/******************************************************************************
* @filename		hittest.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file contains functions for 2D collision detection.
				It includes checks for mouse-over events on rectangles and
				circles, as well as functions to detect intersections between
				two circles, two rectangles, or a circle and a rectangle.
				
				The file provides functionality for updating rectangle
				hitboxes based on an entity's position and direction, and for
				comparing two hitboxes to check if they are the same. These
				functions are essential for handling geometric interactions.
******************************************************************************/

#include "hittest.h"
#include <math.h>
#include <cprocessing.h>
#include <stdio.h>

int is_mouse_on_rect(struct Rectangle rect, float click_x, float click_y) {
	rect.pos.x -= (rect.width / 2);
	rect.pos.y -= (rect.height / 2);

	float xStartpoint = rect.pos.x,
		xEndpoint = rect.pos.x + rect.width,
		yStartpoint = rect.pos.y,
		yEndpoint = rect.pos.y + rect.height;

	return click_x >= xStartpoint && click_x <= xEndpoint &&
		click_y >= yStartpoint && click_y <= yEndpoint;
}

int is_mouse_on_circle(struct Circle circ, float click_x, float click_y) {
	float dx = click_x - circ.pos.x,
		dy = click_y - circ.pos.y,
		distance = (dx * dx) + (dy * dy),
		radius = circ.diameter / 2.f;

	return distance <= (radius * radius);
}

int are_circles_intersecting(struct Circle a, struct Circle b) {
	float distance = CP_Vector_Distance(a.pos, b.pos),
		r1 = a.diameter / 2.f,
		r2 = b.diameter / 2.f;
	return distance < r1 + r2;
}

// Updated for more reliability
int are_rects_intersecting(struct Rectangle a, struct Rectangle b) {
	// Half sizes
	float aHalfWidth = fabsf(a.width) / 2.f,
		aHalfHeight = a.height / 2.f,
		bHalfWidth = fabsf(b.width) / 2.f,
		bHalfHeight = b.height / 2.f;

	// Rectangle sides
	// a
	// b
	float aLeft = a.pos.x - aHalfWidth,
		aRight = a.pos.x + aHalfWidth,
		aTop = a.pos.y - aHalfHeight,
		aBottom = a.pos.y + aHalfHeight;
	float bLeft = b.pos.x - bHalfWidth,
		bRight = b.pos.x + bHalfWidth,
		bTop = b.pos.y - bHalfHeight,
		bBottom = b.pos.y + bHalfHeight;

	// Check for overlap
	return aLeft < bRight && aRight > bLeft && aTop < bBottom && aBottom > bTop;
}

int are_circle_rect_intersecting(struct Circle circ, struct Rectangle rect) {
	if (circ.diameter == 0.f)
		return 0;

	rect.pos.x -= (fabsf(rect.width) / 2);
	rect.pos.y -= (fabsf(rect.height) / 2);

	float radius = fabsf(circ.diameter) / 2.f, // Diameter CANNOT be negative
		nearestX = CP_Math_ClampFloat(circ.pos.x, rect.pos.x, rect.pos.x + fabsf(rect.width)),
		nearestY = CP_Math_ClampFloat(circ.pos.y, rect.pos.y, rect.pos.y + fabsf(rect.height));

	CP_Vector direction = CP_Vector_Subtract(circ.pos, CP_Vector_Set(nearestX, nearestY));
	
	// Penetration depth
	float magnitude = CP_Vector_Length(direction),
		pDepth = radius - magnitude;

	return pDepth > 0;
}

int is_circle_at_boundary(struct Circle circ, float screen_w, float screen_h) {
	float radius = circ.diameter / 2.f;
	return circ.pos.x + radius <= 0 || circ.pos.y + radius <= 0 || circ.pos.x + radius >= screen_w || circ.pos.y + radius >= screen_h;
}

int is_rect_at_boundary(struct Rectangle rect, float screen_w, float screen_h) {
	rect.pos.x -= (rect.width / 2);
	rect.pos.y -= (rect.height / 2);

	return rect.pos.x <= 0 || rect.pos.y <= 0 || rect.pos.x + rect.width >= screen_w || rect.pos.y + rect.height >= screen_h;
}

void update_rect_hitbox(struct Rectangle* rect, float entity_width, CP_Vector base_pos, float offset_x, float offset_y) {
	// Direction (if width is negative, then it's flipped)
	int direction = (entity_width >= 0.f) ? 1 : -1;

	rect->pos.x = base_pos.x + (offset_x * direction);
	rect->pos.y = base_pos.y + offset_y;

	// Mirror the hitbox's width if the entity is facing left
	rect->width = (direction == -1) ? -fabsf(rect->width) : fabsf(rect->width);
}

bool is_same_hitbox(struct Rectangle lhs, struct Rectangle rhs) {
	return (lhs.pos.x == rhs.pos.x) && (lhs.pos.y == rhs.pos.y) && (lhs.width == rhs.width) && (lhs.height == rhs.height);
}

