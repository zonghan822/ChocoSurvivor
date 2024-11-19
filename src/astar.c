#include "cprocessing.h"
#include "astar.h"
#include "utils.h"
#include "hittest.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int _nodeCountX, _nodeCountY;
static float _nodeWidth , _nodeHeight;

int init_grid(PathNode* grid, float levelWidth, float levelHeight, float nodeWidth, float nodeHeight, float spawnWidth) {
	float paddingX = nodeWidth / 2;
	float paddingY = nodeHeight / 2;
	_nodeCountX = (levelWidth + spawnWidth) / nodeWidth; // grid start from the spawn area.
	_nodeCountY = levelHeight / nodeHeight;
	_nodeWidth = nodeWidth;
	_nodeHeight = nodeHeight;
	float marginX = paddingX + (levelWidth - (nodeWidth * _nodeCountX))/2;
	float marginY = paddingY + (levelHeight - (nodeHeight * _nodeCountY))/2;
	int nodeCount = 0;

	for (int i = 0; i < _nodeCountX; i++) {
		for (int j = 0; j < _nodeCountY; j++) {
			CP_Vector nodePos = { (marginX + nodeWidth * i) - spawnWidth/2 , marginY + nodeHeight * j };
			/****************************************** 
			grid indexes are stored in following order:
			e.g.:
			grid[]:			i:				j:
			[0, 1, 2]		[0, 1, 2]		[0, 0, 0]
			[3, 4, 5]		[0, 1, 2]		[1, 1, 1]
			[6, 7, 8]		[0, 1, 2]		[2, 2, 2]
			******************************************/
			int nodeIndex = i + j * _nodeCountX;
			grid[nodeIndex].pos = nodePos;
			nodeCount++;
		}
	}
	return nodeCount;
}

bool is_valid_node(PathNode node, struct Rectangle* worldClips, int clipsSize){
	struct Rectangle currentRect = { node.pos, .width = _nodeWidth, .height = _nodeHeight };
	for (int clipIndex = 0; clipIndex < clipsSize; clipIndex++) {
		if (are_rects_intersecting(currentRect, worldClips[clipIndex])) {
			return false;
			break;
		}
	}
	return true;
}

void update_grid(PathNode* grid, struct Rectangle* worldClips, int clipsSize) {
	// Connection to 8 directions (up, down, left, right, and diagonals)
	for (int i = 0; i < _nodeCountX; i++) {
		for (int j = 0; j < _nodeCountY; j++) {
			int nodeIndex = i + j * _nodeCountX;
			grid[nodeIndex].connectionCount = 0;

			if (is_valid_node(grid[nodeIndex], worldClips, clipsSize)){

				int connectionStartIndexI = max(0, i - 1);
				int connectionStartIndexJ = max(0, j - 1);
				int connectionEndIndexI = min(i + 1, _nodeCountX - 1);
				int connectionEndIndexJ = min(j + 1, _nodeCountY - 1);
				int connectionIndex = 0;

				for (int cI = connectionStartIndexI; cI <= connectionEndIndexI; cI++) {
					for (int cJ = connectionStartIndexJ; cJ <= connectionEndIndexJ; cJ++) {
						int neighbourIndex = cI + cJ * _nodeCountX;
						if (nodeIndex == neighbourIndex) continue;
						if (is_valid_node(grid[neighbourIndex], worldClips, clipsSize)) {
							grid[nodeIndex].connections[connectionIndex] = neighbourIndex;
							grid[nodeIndex].connectionCount++;
							connectionIndex++;
						}
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
		// if the valid node has shorter distance to position
		float distance = CP_Vector_Distance(position, grid[i].pos);
		if (grid[i].connectionCount != 0 &&  distance < closestDistance) {
			closestNodeIndex = i;
			closestDistance = distance;
		}
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


int find_astar_path(PathNode* grid, int nodeCount, int startNodeIndex, int endNodeIndex, int *pathArray) {
	
	AStarNode startNode = { .nodeIndex = startNodeIndex, .fCost = 0.f, .gCost = 0.f, .hCost = 0.f, .parentNode = NULL };
	AStarNode endNode = { .nodeIndex = endNodeIndex, .fCost = 0.f, .gCost = 0.f, .hCost = 0.f, .parentNode = NULL };

	// Initialize open and closed list
	AStarNode openList[MAX_NODE_SIZE];
	AStarNode closedList[MAX_NODE_SIZE];

	// Add start node to open list
	int openListCount = 0, closedListCount = 0;
	openList[openListCount++] = startNode;

	// loop until find end
	while (openListCount > 0) {
		
		// Get the current Node which has the least f cost.
		AStarNode currentNode = openList[0];
		for (int i = 1; i < openListCount; i++) {
			if (openList[i].fCost < currentNode.fCost) {
				currentNode = openList[i];
			}
		}

		// remove current node from open list and add it to closed list
		openListCount = remove_node(openList, currentNode.nodeIndex, openListCount);
		closedList[closedListCount++] = currentNode;

		// Goal is found
		if (currentNode.nodeIndex == endNode.nodeIndex) {
			AStarNode current = currentNode;
			int pathNodeCount = 0;

			// backtrack and store path in array
			while (current.nodeIndex != startNode.nodeIndex) {
				pathArray[pathNodeCount++] = current.nodeIndex;
				current = *(current.parentNode);
			}
			reverse_array(pathArray, pathNodeCount);
			return pathNodeCount;
		}
		

		for (int i = 0; i < grid[currentNode.nodeIndex].connectionCount; i++) {
			
			// ignore if neighbour node in closed list
			bool isNeighbourInClosed = false;
			for (int closedListIndex = 0; closedListIndex < closedListCount; closedListIndex++) {
				if (closedList[closedListIndex].nodeIndex == grid[currentNode.nodeIndex].connections[i]) {
					isNeighbourInClosed = true;
					break;
				}
			}

			if (!isNeighbourInClosed) {
				AStarNode* parentNode = (struct AStarNode*)malloc(sizeof(struct AStarNode));

				parentNode->nodeIndex = currentNode.nodeIndex;
				parentNode->fCost = currentNode.fCost;
				parentNode->gCost = currentNode.gCost;
				parentNode->hCost = currentNode.hCost;
				parentNode->parentNode = currentNode.parentNode;

				// create f, g, h values
				AStarNode neighbourNode = { .nodeIndex = grid[currentNode.nodeIndex].connections[i],
											.parentNode = parentNode, 
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
	return 0;
}