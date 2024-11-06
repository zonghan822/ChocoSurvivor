#include "hittest.h"
#include <math.h>
#include <cprocessing.h>

int is_rect_clicked(struct Rectangle rect, float click_x, float click_y) {
	rect.pos.x -= (rect.width / 2);
	rect.pos.y -= (rect.height / 2);

	float xStartpoint = rect.pos.x,
		xEndpoint = rect.pos.x + rect.width,
		yStartpoint = rect.pos.y,
		yEndpoint = rect.pos.y + rect.height;

	return click_x >= xStartpoint && click_x <= xEndpoint &&
		click_y >= yStartpoint && click_y <= yEndpoint;
}

int is_circle_clicked(struct Circle circ, float click_x, float click_y) {
	float dx = click_x - circ.pos.x,
		dy = click_y - circ.pos.y,
		distance = (dx * dx) + (dy * dy),
		radius = circ.diameter / 2.0;

	return distance <= (radius * radius);
}

int are_circles_intersecting(struct Circle a, struct Circle b) {
	float distance = CP_Vector_Distance(a.pos, b.pos),
		r1 = a.diameter / 2.0,
		r2 = b.diameter / 2.0;
	return distance < r1 + r2;
}

int are_rects_intersecting(struct Rectangle a, struct Rectangle b) {
	a.pos.x -= (a.width / 2);
	a.pos.y -= (a.height / 2);
	b.pos.x -= (b.width / 2);
	b.pos.y -= (b.height / 2);

	float aRx = a.pos.x + a.width, // rx = Right x
		aBy = a.pos.y + a.height, // by = Bottom y
		bRx = b.pos.x + b.width,
		bBy = b.pos.y + b.height;

	// If all true, they are intersecting
	return a.pos.x < bRx && aRx > b.pos.x && a.pos.y < bBy && aBy > b.pos.y;
}

int are_circle_rect_intersecting(struct Circle circ, struct Rectangle rect) {
	rect.pos.x -= (rect.width / 2);
	rect.pos.y -= (rect.height / 2);

	float radius = circ.diameter / 2.0,
		nearestX = CP_Math_ClampFloat(circ.pos.x, rect.pos.x, rect.pos.x + rect.width),
		nearestY = CP_Math_ClampFloat(circ.pos.y, rect.pos.y, rect.pos.y + rect.height);

	CP_Vector direction = CP_Vector_Subtract(circ.pos, CP_Vector_Set(nearestX, nearestY));
	
	// Penetration depth
	float magnitude = CP_Vector_Length(direction),
		pDepth = radius - magnitude;

	return pDepth > 0;
}

int is_circle_at_boundary(struct Circle circ, float screen_w, float screen_h) {
	float radius = circ.diameter / 2.0;
	return circ.pos.x + radius <= 0 || circ.pos.y + radius <= 0 || circ.pos.x + radius >= screen_w || circ.pos.y + radius >= screen_h;
}

int is_rect_at_boundary(struct Rectangle rect, float screen_w, float screen_h) {
	rect.pos.x -= (rect.width / 2);
	rect.pos.y -= (rect.height / 2);

	return rect.pos.x <= 0 || rect.pos.y <= 0 || rect.pos.x + rect.width >= screen_w || rect.pos.y + rect.height >= screen_h;
}

void update_rect_hitbox(struct Rectangle* rect, float resized_width, float resized_height, CP_Vector pos) {
	rect->height = resized_height == 0.0f ? rect->height : resized_height;
	rect->width = resized_width == 0.0f ? rect->width : resized_width;
	rect->pos = pos;
}