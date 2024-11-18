#include "cprocessing.h"
#include "astar.h"
#include "utils.h"
#include "hittest.h"
#include <math.h>
#include <stdio.h>

//float heuristic(PathNode* currentNode, PathNode* endNode) {
//	float dx = fabs(currentNode->pos.x – endNode->pos.x);
//	float dy = fabs(currentNode->pos.y – endNode->pos.y);
//	return D * (dx + dy) + (D2 - 2 * D) * min(dx, dy)
//}

static int _nodeCountX, _nodeCountY;
static float _nodeWidth , _nodeHeight;

int init_grid(PathNode* grid, float levelWidth, float levelHeight, float nodeWidth, float nodeHeight) {
	float paddingX = nodeWidth / 2;
	float paddingY = nodeHeight / 2;
	_nodeCountX = levelWidth / nodeWidth;
	_nodeCountY = levelHeight / nodeHeight;
	_nodeWidth = nodeWidth;
	_nodeHeight = nodeHeight;
	float marginX = paddingX + (levelWidth - (nodeWidth * _nodeCountX))/2;
	float marginY = paddingY + (levelHeight - (nodeHeight * _nodeCountY))/2;
	int nodeCount = 0;

	for (int i = 0; i < _nodeCountX; i++) {
		for (int j = 0; j < _nodeCountY; j++) {
			CP_Vector nodePos = { marginX + nodeWidth * i , marginY + nodeHeight * j };
			int nodeIndex = i + j * _nodeCountX;
			grid[nodeIndex].pos = nodePos;
			nodeCount++;
		}
	}
	return nodeCount;
}

void update_grid(PathNode* grid, struct Rectangle* worldClips, int clipsSize) {
	for (int i = 0; i < _nodeCountX; i++) {
		for (int j = 0; j < _nodeCountY; j++) {
			int connectionStartIndexI = max(0, i - 1);
			int connectionStartIndexJ = max(0, j - 1);
			int connectionEndIndexI = min(i + 1, _nodeCountX - 1);
			int connectionEndIndexJ = min(j + 1, _nodeCountY - 1);
			int nodeIndex = i + j * _nodeCountX;
			int connectionIndex = 0;
			grid[nodeIndex].connectionCount = 0;

			for (int cI = connectionStartIndexI; cI <= connectionEndIndexI; cI++) {
				for (int cJ = connectionStartIndexJ; cJ <= connectionEndIndexJ; cJ++) {
					int neighbourIndex = cI + cJ * _nodeCountX;
					if (nodeIndex == neighbourIndex) continue;
					PathNode neighbourNode = grid[neighbourIndex];
					struct Rectangle neighbourRect = { neighbourNode.pos, .width = _nodeWidth * 2, .height = _nodeHeight * 2 };
					bool isValidNode = true;
					for (int clipIndex = 0; clipIndex < clipsSize; clipIndex++) {
						if (are_rects_intersecting(neighbourRect, worldClips[clipIndex])) {
							isValidNode = false;
							break;
						}
					}
					if (isValidNode) {
						grid[nodeIndex].connections[connectionIndex] = neighbourIndex;
						printf("node %d: neighbour %d\n", nodeIndex, neighbourIndex);
						grid[nodeIndex].connectionCount++;
						connectionIndex++;
					}
				}
			}
		}
	}
}

void draw_grid(PathNode* grid, int nodeCount, struct Camera cam) {
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	for (int i = 0; i < nodeCount; i++) {
		CP_Vector nodeVec = get_cam_pos_rel(grid[i].pos, cam);
		CP_Graphics_DrawCircle(nodeVec.x, nodeVec.y, 20.f);
		for (int cI = 0; cI < grid[i].connectionCount; cI++) {
			CP_Vector neighbourVec = get_cam_pos_rel(grid[grid[i].connections[cI]].pos, cam);
			CP_Graphics_DrawLine(nodeVec.x, nodeVec.y, neighbourVec.x, neighbourVec.y);
		}
	}
}

int get_nearest_node_index(PathNode* grid, int nodeCount, CP_Vector position) {
	int closestNodeIndex = 0;
	float closestDistance = CP_Vector_Distance(position, grid[closestNodeIndex].pos);
	for (int i = 1; i < nodeCount; i++) {
		closestNodeIndex = (CP_Vector_Distance(position, grid[i].pos) < closestDistance) ? i : closestNodeIndex; 
	}
	return closestNodeIndex;
}

//void remove_node(AStarNode* AStarList, int nodeIndex, int nodeCount) {
//	for (int i = 0; i < nodeCount; i++) {
//		if (AStarList[i].nodeIndex == nodeIndex) {
//			
//		}
//	}
//}
//
//bool find_astar_path(PathNode* grid, int nodeCount, CP_Vector startPos, CP_Vector endPos) {
//	AStarNode startNode = { .nodeIndex = get_nearest_node_index(grid, nodeCount, startPos),
//							.gCost = 0.f, .hCost = 0.f };
//	AStarNode endNode = { .nodeIndex = get_nearest_node_index(grid, nodeCount, endPos),
//							.gCost = 0.f, .hCost = 0.f };

	
//}