#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "camera.h"

typedef struct PathNode {
	CP_Vector pos;
	int connections[8];
	int connectionCount;
}PathNode;


typedef struct AStarNode {
	int nodeIndex;
	float gCost;
	float hCost;
	int pathIndex;
	struct AStarNode* previousNode;
}AStarNode;


int init_grid(PathNode* grid, float levelWidth, float levelHeight, float nodeWidth, float nodeHeight);
void update_grid(PathNode* grid, struct Rectangle* worldClips, int clipsSize);
void draw_grid(PathNode* grid, int nodeCount, struct Camera cam); // for debugging