#include "hittest.h"
#include <math.h>
#include <cprocessing.h>

int IsRectClicked(float rect_x, float rect_y, float rect_w, float rect_h, float click_x, float click_y, CP_POSITION_MODE rect_mode)
{
	rect_x = rect_mode == CP_POSITION_CENTER ? rect_x - (rect_w / 2) : rect_x;
	rect_y = rect_mode == CP_POSITION_CENTER ? rect_y - (rect_h / 2) : rect_y;

	float x_startpoint = rect_x,
		x_endpoint = rect_x + rect_w,
		y_startpoint = rect_y,
		y_endpoint = rect_y + rect_h;

	return click_x >= x_startpoint && click_x <= x_endpoint &&
		click_y >= y_startpoint && click_y <= y_endpoint;
}

int IsCircleClicked(float circle_x, float circle_y, float radius, float click_x, float click_y)
{
	float dx = click_x - circle_x,
		dy = click_y - circle_y,
		distance = (dx * dx) + (dy * dy);

	return distance <= (radius * radius);
}

int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2)
{
	CP_Vector c1_vec = CP_Vector_Set(c1_x, c1_y),
		c2_vec = CP_Vector_Set(c2_x, c2_y);
	float distance = CP_Vector_Distance(c1_vec, c2_vec);

	return distance < r1 + r2;
}

int AreRectsIntersecting(float rect1_x, float rect1_y, float rect1_w, float rect1_h, CP_POSITION_MODE rect1_mode,
	float rect2_x, float rect2_y, float rect2_w, float rect2_h, CP_POSITION_MODE rect2_mode)
{
	rect1_x = rect1_mode == CP_POSITION_CENTER ? rect1_x - (rect1_w / 2) : rect1_x;
	rect1_y = rect1_mode == CP_POSITION_CENTER ? rect1_y - (rect1_y / 2) : rect1_y;
	rect2_x = rect2_mode == CP_POSITION_CENTER ? rect2_x - (rect2_w / 2) : rect2_x;
	rect2_y = rect2_mode == CP_POSITION_CENTER ? rect2_x - (rect2_y / 2) : rect2_y;

	float rect1_rx = rect1_x + rect1_w, // rx = Right x
		rect1_by = rect1_y + rect1_h, // by = Bottom y
		rect2_rx = rect2_x + rect2_w,
		rect2_by = rect2_y + rect2_h;

	// If all true, they are intersecting
	return rect1_x < rect2_rx && rect1_rx > rect2_x && rect1_y < rect2_by && rect1_by > rect2_y;
}

int AreCircleRectIntersecting(float circle_x, float circle_y, float radius, float rect_x, float rect_y, float rect_w, float rect_h, CP_POSITION_MODE rect_mode)
{
	rect_x = rect_mode == CP_POSITION_CENTER ? rect_x - (rect_w / 2) : rect_x;
	rect_y = rect_mode == CP_POSITION_CENTER ? rect_y - (rect_h / 2) : rect_y;

	// Calculate the horizontal and vertical distances between the circle's center and the rectangle's center
	float dist_x = fabsf(circle_x - rect_x - rect_w / 2),
		dist_y = fabsf(circle_y - rect_y - rect_h / 2);

	// Check if the distances exceed the rectangle's half-width/height + circle's radius
	if (dist_x > (rect_w / 2 + radius) || dist_y > (rect_h / 2 + radius))
		return 0;

	// Check the corner case for collision
	float dx = dist_x - rect_w / 2,
		dy = dist_y - rect_h / 2,
		dist = (dx * dx) + (dy * dy); // Same as IsCircleClicked()

	// Also: if the distances are within the rectangle's half-width/height, there's a collision
	return dist_x <= (rect_w / 2) || dist_y <= (rect_h / 2) || (dist <= (radius * radius));
}

int IsCircleAtBoundary(float circle_x, float circle_y, float radius, float screen_w, float screen_h)
{
	return circle_x + radius <= 0 || circle_y + radius <= 0 || circle_x + radius >= screen_w || circle_y + radius >= screen_h;
}

int IsRectAtBoundary(float rect_x, float rect_y, float rect_w, float rect_h, float screen_w, float screen_h, CP_POSITION_MODE rect_mode)
{
	rect_x = rect_mode == CP_POSITION_CENTER ? rect_x - (rect_w / 2) : rect_x;
	rect_y = rect_mode == CP_POSITION_CENTER ? rect_y - (rect_y / 2) : rect_y;

	return rect_x <= 0 || rect_y <= 0 || rect_x + rect_w >= screen_w || rect_y + rect_h >= screen_h;
}
