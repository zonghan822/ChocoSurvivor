#include "camera.h"
#include "player.h"
#include "cprocessing.h"

static const float SCALE_FACTOR = 1.2f;
static const float MOVE_FACTOR = 150.f;
static const float MIN_SCALE = 1.f;
static const float MAX_SCALE = 2.5f;

static float _currentScale;
static CP_Vector _centerOffset, _currentPosition;
static CP_Matrix _translationMatrix;

static CP_Matrix _transform;
static float _maxWidth, _maxHeight;

void init_camera(struct ChocoShooter player, float width, float height) {
	_maxWidth = width;
	_maxHeight = height;
	_currentScale = 1.f;

	_currentPosition = CP_Vector_Set(-player.pos.x, -player.pos.y);
	_translationMatrix = CP_Matrix_Translate(_currentPosition);

	_centerOffset = CP_Vector_Set(CP_System_GetWindowWidth() / 2.f, CP_System_GetWindowHeight() / 2.f);
	_transform = CP_Matrix_Identity();
}

void update_camera(struct ChocoShooter player, float dt) {
	const float minDistance = 2.5f; // to smoothen movement later
	// Translation
	if (CP_Vector_Distance(CP_Vector_Negate(_currentPosition), player.pos) < minDistance)
		_currentPosition = CP_Vector_Set(-player.pos.x, -player.pos.y); // Correct the camera so it won't shake continuously
	else { // Continue moving the camera until it reaches the player's pos
		CP_Vector delta = CP_Vector_Subtract(player.pos, CP_Vector_Negate(_currentPosition));
		CP_Vector moveDirection = CP_Vector_Zero();

		if (delta.x != 0.f)
			moveDirection.x = delta.x < 0.f ? -1.f : 1.f;
		if (delta.y != 0.f)
			moveDirection.y = delta.y < 0.f ? -1.f : 1.f;

		// Smoothen movement
		if (delta.x != 0 && (delta.x < minDistance && delta.x > -minDistance)) {
			moveDirection.x = 0.f;
			_currentPosition.x = -player.pos.x;
		}
		if (delta.y != 0 && (delta.y < minDistance && delta.y > -minDistance)) {
			moveDirection.y = 0.f;
			_currentPosition.y = -player.pos.y;
		}

		// Move camera
		if (moveDirection.x != 0)
			moveDirection.x += dt * (moveDirection.x > 0 ? MOVE_FACTOR : -MOVE_FACTOR);
		if (moveDirection.y != 0)
			moveDirection.y += dt * (moveDirection.y > 0 ? MOVE_FACTOR : -MOVE_FACTOR);

		_currentPosition = CP_Vector_Add(_currentPosition, CP_Vector_Set(-moveDirection.x, -moveDirection.y));
	}
	CP_Matrix offsetMatrix = CP_Matrix_Translate(_centerOffset);

	// Clamp
	float windowWidth = CP_System_GetWindowWidth(),
		windowHeight = CP_System_GetWindowHeight();
	_currentPosition.x = CP_Math_ClampFloat(_currentPosition.x, -(_maxWidth - _centerOffset.x), -_centerOffset.x);
	_currentPosition.y = CP_Math_ClampFloat(_currentPosition.y, -(_maxHeight - _centerOffset.y), -_centerOffset.y);

	// Apply matrix
	_translationMatrix = CP_Matrix_Translate(_currentPosition);
	_transform = CP_Matrix_Multiply(_translationMatrix, offsetMatrix);

	CP_Settings_ApplyMatrix(_transform);
}

void apply_camera(void) {
	CP_Settings_ApplyMatrix(_transform);
}