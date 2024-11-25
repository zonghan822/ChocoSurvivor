#pragma once
#include "cprocessing.h"
#include "utils.h"
#include <stdbool.h>

#pragma region Click
int is_mouse_on_rect(struct Rectangle rect, float click_x, float click_y);
int is_mouse_on_circle(struct Circle circ, float click_x, float click_y);
#pragma endregion

#pragma region Intersect
int are_circles_intersecting(struct Circle a, struct Circle b);
int are_rects_intersecting(struct Rectangle a, struct Rectangle b);
int are_circle_rect_intersecting(struct Circle circ, struct Rectangle rect);
int is_circle_at_boundary(struct Circle circ, float screen_w, float screen_h);
int is_rect_at_boundary(struct Rectangle rect, float screen_w, float screen_h);
#pragma endregion

#pragma region Hitbox
void update_rect_hitbox(struct Rectangle* rect, float entity_width, CP_Vector base_pos, float offset_x, float offset_y);
bool is_same_hitbox(struct Rectangle lhs, struct Rectangle rhs);
#pragma endregion