#include "cprocessing.h"
#include "astar.h"
#include "utils.h"
#include "hittest.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


void init_grid(Grid* grid, float levelWidth, float levelHeight, float nodeWidth, float nodeHeight) {
	grid->nodeCountX = (int)(levelWidth / nodeWidth);
	grid->nodeCountY = (int)(levelHeight / nodeHeight);
	grid->nodeCount = grid->nodeCountX * grid->nodeCountY;
	grid->nodeWidth = nodeWidth;
	grid->nodeHeight = nodeHeight;
	grid->nodes = (PathNode*)malloc(grid->nodeCount * sizeof(PathNode));
	if (grid->nodes == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}

	float paddingX = nodeWidth / 2;
	float paddingY = nodeHeight / 2;
	float marginX = paddingX + (levelWidth - (nodeWidth * grid->nodeCountX)) / 2;
	float marginY = paddingY + (levelHeight - (nodeHeight * grid->nodeCountY)) / 2;

	for (int i = 0; i < grid->nodeCountX; i++) {
		for (int j = 0; j < grid->nodeCountY; j++) {
			CP_Vector nodePos = { marginX + nodeWidth * i , marginY + nodeHeight * j };
			/******************************************
			grid indexes are stored in following order:
			(row major)
			e.g.:
			grid[]:			i:				j:
			[0, 1, 2]		[0, 1, 2]		[0, 0, 0]
			[3, 4, 5]		[0, 1, 2]		[1, 1, 1]
			[6, 7, 8]		[0, 1, 2]		[2, 2, 2]
			******************************************/
			int nodeIndex = i + j * grid->nodeCountX;
			grid->nodes[nodeIndex].pos = nodePos;
		}
	}
}

static bool is_valid_node(PathNode node, float nodeWidth, float nodeHeight, struct Rectangle* worldClips, int clipsSize) {
	struct Rectangle currentRect = { node.pos, .width = nodeWidth, .height = nodeHeight };
	for (int clipIndex = 0; clipIndex < clipsSize; clipIndex++) {
		if (are_rects_intersecting(currentRect, worldClips[clipIndex])) {
			return false;
			break;
		}
	}
	return true;
}

void update_grid(Grid* grid, struct Rectangle* worldClips, int clipsSize, struct Rectangle* enemyClips, int enemyClipsSize) {
	int size = clipsSize + enemyClipsSize;
	struct Rectangle* tempArray = (struct Rectangle*)malloc((unsigned long)size * sizeof(struct Rectangle));
	if (tempArray == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}

	for (int i = 0; i < size; i++) {
		if (i < clipsSize)
			tempArray[i] = worldClips[i];
		else
			tempArray[i] = enemyClips[i - clipsSize];
	}

	// Connection to 8 directions (up, down, left, right, and diagonals)
	for (int i = 0; i < grid->nodeCountX; i++) {
		for (int j = 0; j < grid->nodeCountY; j++) {
			int nodeIndex = i + j * grid->nodeCountX;
			grid->nodes[nodeIndex].connectionCount = 0;

			if (is_valid_node(grid->nodes[nodeIndex], grid->nodeWidth, grid->nodeHeight, tempArray, size)) {

				int connectionStartIndexI = max(0, i - 1);
				int connectionStartIndexJ = max(0, j - 1);
				int connectionEndIndexI = min(i + 1, grid->nodeCountX - 1);
				int connectionEndIndexJ = min(j + 1, grid->nodeCountY - 1);
				int connectionIndex = 0;

				for (int cI = connectionStartIndexI; cI <= connectionEndIndexI; cI++) {
					for (int cJ = connectionStartIndexJ; cJ <= connectionEndIndexJ; cJ++) {
						int neighbourIndex = cI + cJ * grid->nodeCountX;
						if (nodeIndex == neighbourIndex) continue;
						if (is_valid_node(grid->nodes[neighbourIndex], grid->nodeWidth, grid->nodeHeight, tempArray, size)) {
							grid->nodes[nodeIndex].connections[connectionIndex] = neighbourIndex;
							grid->nodes[nodeIndex].connectionCount++;
							connectionIndex++;
						}
					}
				}
			}
		}
	}

	// Done, so free memory
	free(tempArray);
}

void draw_grid(PathNode* grid, int nodeCount) {
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	for (int i = 0; i < nodeCount; i++) {
		CP_Graphics_DrawCircle(grid[i].pos.x, grid[i].pos.y, 20.f);
		for (int cI = 0; cI < grid[i].connectionCount; cI++) {
			CP_Graphics_DrawLine(grid[i].pos.x, grid[i].pos.y, grid[grid[i].connections[cI]].pos.x, grid[grid[i].connections[cI]].pos.y);
		}
	}
}

int get_nearest_node_index(PathNode* grid, int nodeCount, CP_Vector position) {
	int closestNodeIndex = 0;
	float closestDistance = CP_Vector_Distance(position, grid[closestNodeIndex].pos);
	for (int i = 1; i < nodeCount; i++) {
		// if the valid node has shorter distance to position
		float distance = CP_Vector_Distance(position, grid[i].pos);
		if (grid[i].connectionCount != 0 && distance < closestDistance) {
			closestNodeIndex = i;
			closestDistance = distance;
		}
	}
	return closestNodeIndex;
}

static void init_list(AStarList* nodeList, int size) {
	nodeList->list = (AStarNode*)malloc(size * sizeof(AStarNode));
	if (nodeList->list == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}
	nodeList->nodeCount = 0;
	nodeList->size = size;
}

static void append_list(AStarList* nodeList, AStarNode node) {
	if (nodeList->nodeCount == nodeList->size) {
		nodeList->size++;
		AStarNode* ptr = (AStarNode*)realloc(nodeList->list, nodeList->size * sizeof(AStarNode));
		if (ptr == NULL) {
			fprintf(stderr, "Memory reallocation failed\n");
			return;
		}
		nodeList->list = ptr;
	}
	nodeList->list[nodeList->nodeCount++] = node;
}

static void pop_list(AStarList* nodeList, AStarNode node) {
	for (int i = 0; i < nodeList->nodeCount; i++) {
		if (nodeList->list[i].nodeIndex == node.nodeIndex) {
			for (int j = i; j < nodeList->nodeCount - 1; j++) {
				nodeList->list[j] = nodeList->list[j + 1];
			}
			nodeList->nodeCount--;
			break;
		}
	}
}

static void reverse_array(int* array, int arrayCount) {
	for (int i = 0, j = arrayCount - 1; i < j; ++i, --j) {
		int tmp = array[i];
		array[i] = array[j];
		array[j] = tmp;
	}
}

int find_astar_path(PathNode* grid, int nodeCount, int startNodeIndex, int endNodeIndex, int* pathArray) {

	AStarNode startNode = { .nodeIndex = startNodeIndex, .fCost = 0.f, .gCost = 0.f, .hCost = 0.f, .parentNode = NULL };
	AStarNode endNode = { .nodeIndex = endNodeIndex, .fCost = 0.f, .gCost = 0.f, .hCost = 0.f, .parentNode = NULL };

	// Initialize open and closed list
	AStarList openList;
	AStarList closedList;
	init_list(&openList, 5);
	init_list(&closedList, 5);

	// Add start node to open list
	append_list(&openList, startNode);

	// loop until find end
	int pathNodeCount = 0;
	while (openList.nodeCount > 0) {

		// Get the current Node which has the least f cost.
		int currentIndex = 0;
		for (int i = 1; i < openList.nodeCount; i++) {
			if (openList.list[i].fCost < openList.list[currentIndex].fCost ||
				(openList.list[i].fCost == openList.list[currentIndex].fCost &&
					openList.list[i].hCost < openList.list[currentIndex].hCost)) {
				currentIndex = i;
			}
		}
		AStarNode currentNode = openList.list[currentIndex];

		// remove current node from open list and add it to closed list
		pop_list(&openList, currentNode);
		append_list(&closedList, currentNode);

		// Goal is found
		if (currentNode.nodeIndex == endNode.nodeIndex) {
			AStarNode current = currentNode;

			// backtrack and store path in array
			while (current.nodeIndex != startNode.nodeIndex) {
				pathArray[pathNodeCount++] = current.nodeIndex;
				if (current.parentNode == NULL)
					break;
				current = *(current.parentNode);
			}
			reverse_array(pathArray, pathNodeCount);
			break;
		}

		for (int i = 0; i < grid[currentNode.nodeIndex].connectionCount; i++) {
			int neighbourIndex = grid[currentNode.nodeIndex].connections[i];

			// ignore if neighbour node in closed list
			bool isNeighbourInClosed = false;
			for (int closedListIndex = 0; closedListIndex < closedList.nodeCount; closedListIndex++) {
				if (closedList.list[closedListIndex].nodeIndex == neighbourIndex) {
					isNeighbourInClosed = true;
					break;
				}
			}

			if (isNeighbourInClosed)
				continue;

			// Check if neighbour is in open list
			int existingNodeIndex = -1;
			for (int i = 0; i < openList.nodeCount; i++) {
				if (openList.list[i].nodeIndex == neighbourIndex)
					existingNodeIndex = i;
			}
			if (existingNodeIndex > -1) {
				if (currentNode.gCost < openList.list[existingNodeIndex].gCost) {
					if (openList.list[existingNodeIndex].parentNode != NULL)
						free(openList.list[existingNodeIndex].parentNode);

					AStarNode* parentNode = (AStarNode*)malloc(sizeof(AStarNode));
					if (parentNode == NULL)
						return 0;
					*parentNode = currentNode;

					openList.list[existingNodeIndex].gCost = currentNode.gCost;
					openList.list[existingNodeIndex].fCost = currentNode.gCost + openList.list[existingNodeIndex].hCost;
					openList.list[existingNodeIndex].parentNode = parentNode;
				}
				continue;
			}

			// Create new AStarNode for neighbour
			AStarNode* parentNode = (AStarNode*)malloc(sizeof(AStarNode));
			if (parentNode == NULL) {
				fprintf(stderr, "Memory allocation failed\n");
				return 0;
			}

			parentNode->nodeIndex = currentNode.nodeIndex;
			parentNode->fCost = currentNode.fCost;
			parentNode->gCost = currentNode.gCost;
			parentNode->hCost = currentNode.hCost;
			parentNode->parentNode = currentNode.parentNode;

			// create f, g, h values
			AStarNode neighbourNode = { .nodeIndex = grid[currentNode.nodeIndex].connections[i],
										.parentNode = parentNode,
										.gCost = currentNode.gCost + 1.f,
										.hCost = CP_Vector_Distance(grid[grid[currentNode.nodeIndex].connections[i]].pos, grid[endNode.nodeIndex].pos),
			};
			neighbourNode.fCost = neighbourNode.gCost + neighbourNode.hCost;

			append_list(&openList, neighbourNode);
		}
	}

	for (int i = 0; i < openList.nodeCount; i++) {
		if (openList.list[i].parentNode != NULL &&
			openList.list[i].parentNode->nodeIndex >= 0 &&
			openList.list[i].parentNode->nodeIndex < nodeCount) {
			free(openList.list[i].parentNode);
		}
	}
	free(openList.list);
	for (int i = 0; i < closedList.nodeCount; i++) {
		if (closedList.list[i].parentNode != NULL &&
			closedList.list[i].parentNode->nodeIndex > 0 &&
			closedList.list[i].parentNode->nodeIndex < nodeCount) {
			free(closedList.list[i].parentNode);
		}
	}
	free(closedList.list);

	return pathNodeCount;
}