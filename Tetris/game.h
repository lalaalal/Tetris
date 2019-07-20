#pragma once

#include <stdio.h>
#include <stdbool.h>

#include <stdlib.h>
#include <time.h>

#include "util.h"

#define NUM_ROW 12
#define NUM_COL 25

#define NUM_ROTATE	4
#define NUM_TILE	4

typedef char Board[NUM_COL][NUM_ROW];

typedef struct {
	Pos pos;
	int blockNum;
	int rotate;
} Block;

static const Pos BOARD_OFFSET = { 1, 1 };
static const Pos NEXT_OFFSET = { NUM_ROW + 5, 1 };
static const Pos HOLD_OFFSET = { NUM_ROW + 5, 9 };

static const Pos BLOCK[][NUM_ROTATE][NUM_TILE] = {
	{
		{ { 0, 0 }, { 1, 0 }, { 2, 0 }, {-1, 0 } },
		{ { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1,-1 } },
		{ { 0, 1 }, { 1, 1 }, { 2, 1 }, {-1, 1 } },
		{ { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0,-1 } }
	},
	{
		{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
		{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
		{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
		{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }
	},
	{
		{ { 0, 0 }, { 1, 0 }, {-1, 0 }, { 0,-1 } },
		{ { 0, 0 }, { 0, 1 }, { 0,-1 }, { 1, 0 } },
		{ { 0, 0 }, { 1, 0 }, {-1, 0 }, { 0, 1 } },
		{ { 0, 0 }, { 0, 1 }, { 0,-1 }, {-1, 0 } },
	},
	{
		{ { 0, 0 }, { 1, 0 }, {-1, 0 }, { 1,-1 } },
		{ { 0, 0 }, { 0, 1 }, { 0,-1 }, { 1, 1 } },
		{ { 0, 0 }, { 1, 0 }, {-1, 0 }, {-1, 1 } },
		{ { 0, 0 }, { 0, 1 }, { 0,-1 }, {-1,-1 } },
	},
	{
		{ { 0, 0 }, { 1, 0 }, {-1, 0 }, {-1,-1 } },
		{ { 0, 0 }, { 0, 1 }, { 0,-1 }, { 1,-1 } },
		{ { 0, 0 }, { 1, 0 }, {-1, 0 }, { 1, 1 } },
		{ { 0, 0 }, { 0, 1 }, { 0,-1 }, {-1, 1 } },
	},
	{
		{ { 0, 0 }, {-1,-1 }, { 0,-1 }, { 1, 0 } },
		{ { 0, 0 }, { 1,-1 }, { 1, 0 }, { 0, 1 } },
		{ { 0, 0 }, {-1,-1 }, { 0,-1 }, { 1, 0 } },
		{ { 0, 0 }, { 1,-1 }, { 1, 0 }, { 0, 1 } }
	},
	{
		{ { 0, 0 }, {-1, 0 }, { 0,-1 }, { 1,-1 } },
		{ { 0, 0 }, { 0,-1 }, { 1, 0 }, { 1, 1 } },
		{ { 0, 0 }, {-1, 0 }, { 0,-1 }, { 1,-1 } },
		{ { 0, 0 }, { 0,-1 }, { 1, 0 }, { 1, 1 } }
	}
};
static const int NUM_BLOCKS = sizeof(BLOCK) / (NUM_ROTATE * NUM_TILE * sizeof(Pos));

enum { ERASE, DRAW, SHADOW, TEST };
enum { PLAYING, GAME_OVER };
enum { NOT_HOLDING = -1, HOLDED = -2, HOLDING };

void play();

Block newBlock();

void init();
void displayGuid();
void drawOutLine(int width, int height, Pos offset);
void displayBoard(const Board board);
void displayBlock(Block block, int blockType);
void displayNextBlock(Block next, int blockType);
bool setBlock(Board board, Block block, int value);
bool moveBlock(Board board, Block* block, int key);
bool rotateBlock(Board board, Block* block);
void hardDown(Board board, Block* block, int blockType);
void refreshShadow(Board board, Block* block, Block* shadow);

void holdBlock(Board board, Block block, Block* hold);
void getBlock(Board board, Block* block, Block* hold);
void displayHold(Block* hold, int blockType);

void setHoldStat(Block* hold, int stat);
int getHoldStat(Block hold);

int dropBlock(Board board, Block block, Block* hold);
void displayLine(const Board board, int col, const char* block);
void pullLine(Board board, int from);
void removeLineAnimation(const Board board, int* cols, int numCol);
void removeLine(Board board, int* cols, int numCol);
void checkLine(Board board);