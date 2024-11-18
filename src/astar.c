#include "cprocessing.h"
#include "astar.h"
#include "utils.h"
#include "hittest.h"
#include <math.h>
#include <stdio.h>

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
					struct Rectangle neighbourRect = { neighbourNode.pos, .width = _nodeWidth, .height = _nodeHeight };
					if (cI - i > 0)
						neighbourRect.pos.x -= neighbourRect.width / 2;
					if (cI - i < 0)
						neighbourRect.pos.x += neighbourRect.width / 2;
					if (cJ - j > 0)
						neighbourRect.pos.y -= neighbourRect.height / 2;
					if (cJ - j < 0)
						neighbourRect.pos.y += neighbourRect.height / 2;

					bool isValidNode = true;
					for (int clipIndex = 0; clipIndex < clipsSize; clipIndex++) {
						if (are_rects_intersecting(neighbourRect, worldClips[clipIndex])) {
							isValidNode = false;
							break;
						}
					}
					if (isValidNode) {
						grid[nodeIndex].connections[connectionIndex] = neighbourIndex;
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

int remove_node(AStarNode* AStarList, int nodeIndex, int nodeCount) {
	for (int i = 0; i < nodeCount; i++) {
		if (AStarList[i].nodeIndex == nodeIndex) {
			int remainingCount = nodeCount - i;
			while (--remainingCount) {
				AStarNode temp = AStarList[i];
				AStarList[i] = AStarList[i + 1];
				AStarList[i + 1] = temp;
				i++;
			}
			nodeCount--;
			break;
		}
	}
	return nodeCount;
}

void reverse_array(int* array, int arrayCount) {
	for (int i = 0, j = arrayCount - 1; i < j; ++i, --j) {
		int tmp = array[i];
		array[i] = array[j];
		array[j] = tmp;
	}
}


int find_astar_path(PathNode* grid, int nodeCount, CP_Vector startPos, CP_Vector endPos, int *pathArray) {
	
	AStarNode startNode = { .nodeIndex = get_nearest_node_index(grid, nodeCount, startPos),
							.fCost = 0.f, .gCost = 0.f, .hCost = 0.f, .parentNode = NULL };
	AStarNode endNode = { .nodeIndex = get_nearest_node_index(grid, nodeCount, endPos),
							.fCost = 0.f, .gCost = 0.f, .hCost = 0.f, .parentNode = NULL };


	AStarNode openList[MAX_NODE_SIZE];
	AStarNode closedList[MAX_NODE_SIZE];

	int openListCount = 0, closedListCount = 0;
	openList[openListCount++] = startNode;


	while (openListCount > 0) {
		
		// Get the current Node which has the least f cost.
		AStarNode currentNode = openList[0];

		for (int i = 1; i < openListCount; i++) {
			if (openList[i].fCost < currentNode.fCost) {
				currentNode = openList[i];
			}
		}

		openListCount = remove_node(openList, currentNode.nodeIndex, openListCount);
		closedList[closedListCount++] = currentNode;

		// Goal is found
		if (currentNode.nodeIndex == endNode.nodeIndex) {
			AStarNode current = currentNode;
			int pathNodeCount = 0;
			while (current.nodeIndex != startNode.nodeIndex) {
				pathArray[pathNodeCount++] = current.nodeIndex;
				current = *(current.parentNode);
			}

			return pathNodeCount;
		}

		for (int i = 0; i < grid[currentNode.nodeIndex].connectionCount; i++) {
			bool isNeighbourInClosed = false;
			for (int closedListIndex = 0; closedListIndex < closedListCount; closedListIndex++) {
				if (closedList[closedListIndex].nodeIndex == grid[currentNode.nodeIndex].connections[i]) {
					isNeighbourInClosed = true;
					break;
				}
			}

			if (!isNeighbourInClosed) {
				AStarNode neighbourNode = { .nodeIndex = grid[currentNode.nodeIndex].connections[i],
											.parentNode = &currentNode, 
											.gCost = currentNode.gCost+1, 
											.hCost = CP_Vector_Distance(grid[grid[currentNode.nodeIndex].connections[i]].pos, grid[endNode.nodeIndex].pos),
				};
				neighbourNode.fCost = neighbourNode.gCost + neighbourNode.hCost;

				bool addNeighbourToOpen = true;
				for (int openListIndex = 0; openListIndex < openListCount; openListIndex++) {
					if ((openList[i].nodeIndex == neighbourNode.nodeIndex) && (openList[i].gCost < neighbourNode.gCost)) {
						addNeighbourToOpen = false;
					}
				}
				
				if (addNeighbourToOpen)
					openList[openListCount++] = neighbourNode;
			}
		}
	}
}