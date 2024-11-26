/******************************************************************************
*@filename		astar.h
*@author(s)		Law Zong Han
*@DP emails		zonghan.law@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This file is a header file for astar pathfinding algorithm for enemy and boss 
movement. It contains functions to create grid for the map layout to integrate
with the astar pathfinding implementation.
******************************************************************************/

#pragma once
#include <stdbool.h>
#include "cprocessing.h"

#define MAX_NODE_SIZE 500

// Nodes in grid with connections
typedef struct PathNode {
	CP_Vector pos;
	int connections[8];
	int connectionCount;
}PathNode;

// For storing nodes to form a map grid
typedef struct Grid {
	PathNode* nodes;
	int nodeCount;
	int nodeCountX, nodeCountY;
	float nodeWidth, nodeHeight;
}Grid;

// For node in astar pathfinding
typedef struct AStarNode {
	int nodeIndex;
	float fCost;
	float gCost;
	float hCost;
	struct AStarNode* parentNode;
}AStarNode;

// For open and closed list in astar pathfinding
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

/****************************************************
 @brief to get the nearest node based on the position 
	given
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

void draw_grid(PathNode* grid, int nodeCount); // draw grid for debugging