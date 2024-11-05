#pragma once

#include "cprocessing.h"

#pragma region Click
int IsRectClicked(float rect_x, float rect_y, float rect_w, float rect_h, float click_x, float click_y, CP_POSITION_MODE rect_mode);
int IsCircleClicked(float circle_x, float circle_y, float radius, float click_x, float click_y);
#pragma endregion

#pragma region Intersect
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2);
int AreRectsIntersecting(float rect1_x, float rect1_y, float rect1_w, float rect1_h, CP_POSITION_MODE rect1_mode, float rect2_x, float rect2_y, float rect2_w, float rect2_h, CP_POSITION_MODE rect2_mode);
int AreCircleRectIntersecting(float circle_x, float circle_y, float radius, float rect_x, float rect_y, float rect_w, float rect_h, CP_POSITION_MODE rect_mode);
int IsCircleAtBoundary(float circle_x, float circle_y, float radius, float screen_w, float screen_h);
int IsRectAtBoundary(float rect_x, float rect_y, float rect_w, float rect_h, float screen_w, float screen_h, CP_POSITION_MODE rect_mode);
#pragma endregion