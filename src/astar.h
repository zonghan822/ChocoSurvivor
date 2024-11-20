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

typedef struct Grid {
	PathNode* nodes;
	int nodeCount;
	int nodeCountX, nodeCountY;
	float nodeWidth, nodeHeight;
}Grid;

typedef struct AStarNode {
	int nodeIndex;
	float fCost;
	float gCost;
	float hCost;
	struct AStarNode* parentNode;
}AStarNode;

typedef struct AStarNodeList {
	AStarNode* list;
	int nodeCount;
	int size;
}AStarList;


/****************************************************
 @brief generate grid based on map size
 @param grid - grid passed in to store the node of the
	map grid
 @param nodeWidth, nodeHeight - size of the node
 @param levelWidth, levelHeight - size of the map
*****************************************************/
void init_grid(Grid* grid, float levelWidth, float levelHeight, float nodeWidth, float nodeHeight);

/****************************************************
 @brief update the connection of nodes in the grid.
	Non-movable path will not have connection.
 @param grid - grid containing the path node of the
	map grid, count of nodes and size of nodes
 @param worldClips, enemyClips - array of non-movable path
 @param clipsSize, enemyClipsSize - size of worldClips 
	array
*****************************************************/
void update_grid(Grid* grid, struct Rectangle* worldClips, int clipsSize, struct Rectangle* enemyClips, int enemyClipsSize);

void draw_grid(PathNode* grid, int nodeCount, struct Camera cam); // draw grid for debugging

/****************************************************
 @param grid - array containing the path node of the
	map grid
 @param nodeCount - count of node in grid
 @param position - a position to search for nearest
	node
 @return return the nearest node to position 
*****************************************************/
int get_nearest_node_index(PathNode* grid, int nodeCount, CP_Vector position);

/****************************************************
 @brief calculate the shortest path from startNode to 
	endnode
 @param grid - array containing the path node of the
	map grid
 @param nodeCount - count of node in grid
 @param startNodeIndex, endNodeIndex - index start and 
	end node in the grid to search shortest path
 @param pathArray - array passed in to store the path
	node index
 @return the path node count, 0 if no path found
*****************************************************/
int find_astar_path(PathNode* grid, int nodeCount, int startNodeIndex, int endNodeIndex, int* pathArray);