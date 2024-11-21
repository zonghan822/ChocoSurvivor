#pragma once
#include "hittest.h"
#include "cprocessing.h"

enum ProgressBarColor {
	PROGRESSBAR_COLOR_RED,
	PROGRESSBAR_COLOR_BLUE,
	PROGRESSBAR_COLOR_GREEN
};

struct ProgressBar {
	struct Rectangle track;
	float value;
	enum ProgressBarColor barColor;
	CP_Color baseColor;
};

struct ProgressBar init_progressbar(float width, float height, float x, float y, CP_Color baseColor);
void update_progressbar(struct ProgressBar* progressBar, float value, float max);
void render_progressbar(struct ProgressBar progressBar);
void render_progressbar_custom(float width, float height, float x, float y, CP_Color baseColor, float value, float max);
void dispose_progressbar(struct ProgressBar* progressBar);