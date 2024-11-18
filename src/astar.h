#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "camera.h"

#define MAX_NODE_SIZE 500

typedef struct PathNode {
	CP_Vector pos;
	int connections[8];
	int connectionCount;
}PathNode;


typedef struct AStarNode {
	int nodeIndex;
	float fCost;
	float gCost;
	float hCost;
	struct AStarNode* parentNode;
}AStarNode;


int init_grid(PathNode* grid, float levelWidth, float levelHeight, float nodeWidth, float nodeHeight);
void update_grid(PathNode* grid, struct Rectangle* worldClips, int clipsSize);
void draw_grid(PathNode* grid, int nodeCount, struct Camera cam); // for debugging

int find_astar_path(PathNode* grid, int nodeCount, CP_Vector startPos, CP_Vector endPos, int* pathArray);