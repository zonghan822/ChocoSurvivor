#pragma once

#include "cprocessing.h"

#pragma region Click
/***********************************************************
  * @brief Checks whether the x and y coordinates of mouse
  *		   click is within the rectangle.
  * @param rect_x: The x coordinates of the rectangle.
  *	@param rect_y: The y coordinates of the rectangle.
  *	@param rect_w: The width of the rectangle.
  *	@param rect_h: The height of the rectangle.
  *	@param click_x: The x coordinates of the mouse click.
  *	@param click_y: The y coordinates of the mouse click.
  * @param rect_mode = The CP_POSITION_MODE in which the
  *		   rectangle was rendered with.
  * @return True (1) or False (0). 
************************************************************/
int IsRectClicked(float rect_x, float rect_y, float rect_w, float rect_h, float click_x, float click_y, CP_POSITION_MODE rect_mode);

/***********************************************************
  * @brief Checks whether the x and y coordinates of mouse
  *	       click is within the circle.
  * @param circle_x: The x coordinates of the circle.
  *	@param circle_y: The y coordinates of the rectangle.
  *	@param radius: The radius of the circle (1/2 of diameter)
  *	@param click_x: The x coordinates of the mouse click.
  *	@param click_y: The y coordinates of the mouse click.
  * @return True (1) or False (0).
************************************************************/
int IsCircleClicked(float circle_x, float circle_y, float radius, float click_x, float click_y);
#pragma endregion

#pragma region Intersect
/***********************************************************
  * @brief Checks whether two circles are intersecting.
  * @param c1_x: The x coordinates of the first circle.
  *	@param c1_y: The y coordinates of the first circle.
  *	@param r1: The radius of the first circle (1/2 of diameter)
  *	@param c2_x: The x coordinates of the second circle.
  *	@param c2_y: The y coordinates of the second circle.
  * @param r2: The radius of the second circle (1/2 of diameter)
  * @return True (1) or False (0).
************************************************************/
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2);

/***********************************************************
  * @brief Checks whether two rectangles are intersecting.
  * @param rect1_x: The x coordinates of the first rectangle.
  *	@param rect1_y: The y coordinates of the first rectangle.
  *	@param rect1_w: The width of the first rectangle.
  *	@param rect1_h: The height of the first rectangle.
  * @param rect1_mode: The CP_POSITION_MODE in which the
  *		   first rectangle was rendered with. 
  * @param rect2_x: The x coordinates of the second rectangle.
  *	@param rect2_y: The y coordinates of the second rectangle.
  *	@param rect2_w: The width of the second rectangle.
  *	@param rect2_h: The height of the second rectangle.
  * @param rect2_mode: The CP_POSITION_MODE in which the
  *		   second rectangle was rendered with. 
  * @return True (1) or False (0).
************************************************************/
int AreRectsIntersecting(float rect1_x, float rect1_y, float rect1_w, float rect1_h, CP_POSITION_MODE rect1_mode, float rect2_x, float rect2_y, float rect2_w, float rect2_h, CP_POSITION_MODE rect2_mode);
int AreCircleRectIntersecting(float circle_x, float circle_y, float radius, float rect_x, float rect_y, float rect_w, float rect_h, CP_POSITION_MODE rect_mode);
int IsCircleAtBoundary(float circle_x, float circle_y, float radius, float screen_w, float screen_h);
int IsRectAtBoundary(float rect_x, float rect_y, float rect_w, float rect_h, float screen_w, float screen_h, CP_POSITION_MODE rect_mode);
#pragma endregion