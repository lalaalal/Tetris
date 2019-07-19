/* 
 * Tetris
 * Date : 2019.07.20
 * by lalaalal
 */

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

const Pos OFFSET = { 1, 1 };
const Pos BOARD_OFFSET = { 0, 0 };
const Pos NEXT_OFFSET = { NUM_ROW + 4, 1 };
const Pos HOLD_OFFSET = { NUM_ROW + 4, 9 };

const Pos BLOCK[][NUM_ROTATE][NUM_TILE] = {
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
const int NUM_BLOCKS = sizeof(BLOCK) / (NUM_ROTATE * NUM_TILE * sizeof(Pos));

enum { ERASE, DRAW, SHADOW, TEST };
enum { PLAYING, GAME_OVER };

Block newBlock();

void init();
void displayGuid();
void drawOutLine(int width, int height, Pos offset);
void displayBoard(const Board board);
void displayBlock(Block block, int blockType);
void displayNextBlock(Block next, int blockType);
bool setBlock(Board board, Block block, int value);
bool moveBlock(Board board, Block * block, int key);
bool rotateBlock(Board board, Block* block);
void hardDown(Board board, Block* block, int blockType);
void refreshShadow(Board board, Block* block, Block* shadow);

void holdBlock(Board board, Block block, Block* hold);
void getBlock(Board board, Block* block, Block* hold);
void displayHold(Block* hold, int blockType);

int dropBlock(Board board, Block block, Block * hold);
void displayLine(const Board board, int col, const char* block);
void pullLine(Board board, int from);
void removeLineAnimation(const Board board, int* cols, int numCol);
void removeLine(Board board, int* cols, int numCol);
void checkLine(Board board);

int main(int argc, char* argv[]) {
	init();
	Board board = { 0, };

	Block curr;
	Block next = newBlock();
	Block hold;
	hold.blockNum = -1;

	srand((unsigned int)time(NULL));
	do {
		curr = next;
		displayNextBlock(next, ERASE);
		next = newBlock();
		displayNextBlock(next, DRAW);

		checkLine(board);
		displayBoard(board);

		if (hold.blockNum == -2)
			hold.blockNum = -1;
	} while (dropBlock(board, curr, &hold) == PLAYING);

	system("pause");

	return 0;
}

// DEBUG
void displayBoardData(const Board board) {
	gotoxy(0, 0);
	for (int y = 0; y < NUM_COL; y++) {
		for (int x = 0; x < NUM_ROW; x++) {
			printf("%2d", board[y][x]);
		}
		printf("\n");
	}
}

Block newBlock() {
	Block new;

	new.pos.x = NUM_ROW / 2;
	new.pos.y = 1;
	new.blockNum = rand() % NUM_BLOCKS;
	new.rotate = rand() % NUM_ROTATE;

	return new;
}

void init() {
	hideCursor();
	setFontColor(GRAY);

	drawOutLine(NUM_ROW + 2, NUM_COL + 2, BOARD_OFFSET);
	drawOutLine(8, 6, HOLD_OFFSET);
	drawOutLine(8, 6, NEXT_OFFSET);

	setFontColor(WHITE);
	displayGuid();
}

void displayGuid() {
	gotoxy(NUM_ROW + 4, 0);
	printf("NEXT");
	gotoxy(NUM_ROW + 4, 8);
	printf("HOLD");

	gotoxy(NUM_ROW + 4, 16);
	printf("  ¡â    : Rotate");
	gotoxy(NUM_ROW + 4, 17);
	printf("¢·  ¢¹  : Move");
	gotoxy(NUM_ROW + 4, 18);
	printf("  ¡ä    : Move");
	gotoxy(NUM_ROW + 4, 19);
	printf("P : Hard Down");
	gotoxy(NUM_ROW + 4, 20);
	printf("H : Hold");
	gotoxy(NUM_ROW + 4, 22);
	printf("Run in Legacy Mode or PowerShell");
	gotoxy(NUM_ROW + 4, 24);
	printf("By Ã¢Çù");
}

void drawOutLine(int width, int height, Pos offset) {
	gotoxy(offset.x, offset.y);
	for (int x = 0; x < width; x++)
		printf("¡á");
	for (int y = 1; y < height - 1; y++) {
		gotoxy(offset.x, y + offset.y);
		printf("¡á");
		for (int x = 0; x < width - 2; x++)
			printf("  ");
		printf("¡á\n");
	}
	gotoxy(offset.x, offset.y + height - 1);
	for (int x = 0; x < width; x++)
		printf("¡á");
}

void displayBoard(const Board board) {
	for (int y = 0; y < NUM_COL; y++) {
		gotoxy(OFFSET.x, y + OFFSET.y);
		for (int x = 0; x < NUM_ROW; x++) {
			if (board[y][x] == 1)
				printf("¡á");
			else if (board[y][x] == 2)
				printf("¡à");
			else
				printf("  ");
		}
	}
}

void displayBlock(Block block, int blockType) {
	Pos pos = block.pos;
	int blockNum = block.blockNum;
	int rotate = block.rotate;

	for (int i = 0; i < 4; i++) {
		int x_pos = pos.x + BLOCK[blockNum][rotate][i].x;
		int y_pos = pos.y + BLOCK[blockNum][rotate][i].y;

		gotoxy(x_pos + OFFSET.x, y_pos + OFFSET.y);
		if (blockType == DRAW)
			printf("¡á");
		else if (blockType == SHADOW)
			printf("¡à");
		else
			printf("  ");
	}
}

void displayNextBlock(Block next, int blockType) {
	next.pos.x = NEXT_OFFSET.x + 2;
	next.pos.y = NEXT_OFFSET.y + 1;
	displayBlock(next, blockType);
}

bool setBlock(Board board, Block block, int value) {
	Pos pos = block.pos;
	int blockNum = block.blockNum;
	int rotate = block.rotate;

	for (int i = 0; i < 4; i++) {
		int x_pos = pos.x + BLOCK[blockNum][rotate][i].x;
		int y_pos = pos.y + BLOCK[blockNum][rotate][i].y;
		if (x_pos < 0 || x_pos >= NUM_ROW ||
			y_pos < 0 || y_pos >= NUM_COL ||
			(board[y_pos][x_pos] == 1 && value))
			return false;
		if (value != TEST)
			board[y_pos][x_pos] = value;
	}
	return true;
}

bool moveBlock(Board board, Block * block, int key) {
	int xMove = 0;
	int yMove = 0;

	if (key == DOWN)  yMove = 1;
	else if (key == LEFT)  xMove = -1;
	else if (key == RIGHT) xMove = 1;
	else return false;

	setBlock(board, *block, ERASE);
	displayBlock(*block, ERASE);
	block->pos.x += xMove;
	block->pos.y += yMove;
	if (!setBlock(board, *block, TEST)) {
		block->pos.x -= xMove;
		block->pos.y -= yMove;
		setBlock(board, *block, DRAW);
		displayBlock(*block, DRAW);

		return false;
	}
	setBlock(board, *block, DRAW);

	return true;
}

bool rotateBlock(Board board, Block * block) {
	setBlock(board, *block, ERASE);
	displayBlock(*block, ERASE);
	block->rotate++;
	block->rotate %= 4;

	if (!setBlock(board, *block, TEST)) {
		block->rotate--;
		block->rotate %= 4;
		setBlock(board, *block, DRAW);
		displayBlock(*block, DRAW);

		return false;
	}
	setBlock(board, *block, DRAW);
	displayBlock(*block, DRAW);

	return true;
}

void hardDown(Board board, Block * block, int blockType) {
	Block tmp = *block;

	setBlock(board, *block, ERASE);
	displayBlock(*block, ERASE);
	while (setBlock(board,* block, TEST))
		block->pos.y++;

	block->pos.y--;
	setBlock(board, *block, blockType);
	displayBlock(*block, blockType);
	if (blockType == SHADOW)
		setBlock(board, tmp, DRAW);
}

void refreshShadow(Board board, Block * block, Block * shadow) {
	setBlock(board, *shadow, ERASE);
	displayBlock(*shadow, ERASE);
	*shadow = *block;
	hardDown(board, shadow, SHADOW);
	displayBlock(*block, DRAW);
}

void holdBlock(Board board, Block block, Block* hold) {
	setBlock(board, block, ERASE);
	displayBlock(block, ERASE);
	*hold = block;
	displayHold(hold, DRAW);
}

void getBlock(Board board, Block* block, Block* hold) {
	Pos curPos = block->pos;

	setBlock(board, *block, ERASE);	
	displayBlock(*block, ERASE);
	*block = *hold;
	block->pos = curPos;
	displayHold(hold, ERASE);
	hold->blockNum = -2;
}

void displayHold(Block * hold, int blockType) {
	Pos offset = { HOLD_OFFSET.x + 2, HOLD_OFFSET.y + 1 };
	hold->pos = offset;
	displayBlock(*hold, blockType);
}

int dropBlock(Board board, Block block, Block * hold) {
	if (!setBlock(board, block, DRAW))
		return GAME_OVER;

	Block shadow = block;
	refreshShadow(board, &block, &shadow);

	do {
		displayBlock(block, DRAW);
		for (int i = 0; i < 100; i++) {
			Sleep(5);
			int key = keyboard();
			
			switch (key) {
			case UP:
				rotateBlock(board, &block);
				refreshShadow(board, &block, &shadow);
				break;
			case DOWN:
			case LEFT:
			case RIGHT:
				moveBlock(board, &block, key);
				refreshShadow(board, &block, &shadow);
				break;
			case 'p':
				hardDown(board, &block, DRAW);
				return PLAYING;
			case 'h':
			case 'H':
				if (hold->blockNum == -1) {
					holdBlock(board, block, hold);
					setBlock(board, shadow, ERASE);
					return PLAYING;
				} if (hold->blockNum == -2) {
					break;
				} else {
					getBlock(board, &block, hold);
					refreshShadow(board, &block, &shadow);
				}
			default:
				break;
			}
		}
	} while (moveBlock(board, &block, DOWN));
	return PLAYING;
}


void displayLine(const Board board, int col, const char * block) {
	gotoxy(OFFSET.x, col + OFFSET.y);
	for (int x = 0; x < NUM_ROW; x++) {
		printf(block);
	}
}

void pullLine(Board board, int from) {
	for (int y = from; y > 0; y--) {
		for (int x = 0; x < NUM_ROW; x++)
			board[y][x] = board[y - 1][x];
	}
}

void removeLineAnimation(const Board board, int * cols, int numCol) {
	for (int t = 0; t < 5; t++) {
		for (int i = 0; i < numCol; i++) {
			int y = cols[i];
			if (t % 2 == 0)
				displayLine(board, y, "¡á");
			else
				displayLine(board, y, "  ");
		}
		Sleep(150);
	}
}

void removeLine(Board board, int * cols, int numCol) {
	for (int i = 0; i < numCol; i++) {
		int y = cols[i];

		for (int x = 0; x < NUM_ROW; x++)
			board[y + i][x] = 0;
		pullLine(board, y + i);
	}
}

void checkLine(Board board) {
	int removeCols[4] = { 0, };
	int lineCount = 0;
	for (int y = NUM_COL - 1; y > 1; y--) {
		for (int x = 0; x < NUM_ROW; x++) {
			if (board[y][x] != 1)
				break;
			if (x + 1 == NUM_ROW)
				removeCols[lineCount++] = y;
		}
	}
	removeLine(board, removeCols, lineCount);
	if (lineCount != 0)
		removeLineAnimation(board, removeCols, lineCount);
}
