#include "game.h"

void play() {
	init();
	Board board = { 0, };

	Block curr;
	Block next = newBlock();
	Block hold;
	setHoldStat(&hold, NOT_HOLDING);

	srand((unsigned int)time(NULL));
	do {
		checkLine(board);
		displayBoard(board);

		curr = next;
		displayNextBlock(next, ERASE);
		next = newBlock();
		displayNextBlock(next, DRAW);

		if (getHoldStat(hold) == HOLDED)
			setHoldStat(&hold, NOT_HOLDING);
	} while (dropBlock(board, curr, &hold) == PLAYING);
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
	system("cls");
	hideCursor();
	setFontColor(GRAY);

	drawOutLine(NUM_ROW + 2, NUM_COL + 2, BOARD_OFFSET);
	drawOutLine(8, 6, HOLD_OFFSET);
	drawOutLine(8, 6, NEXT_OFFSET);

	setFontColor(WHITE);
	displayGuid();
}

void displayGuid() {
	Pos offset = { NUM_ROW + 4 + BOARD_OFFSET.x, BOARD_OFFSET.y };
	printxy(offset.x, offset.y, "NEXT");
	gotoxy(offset.x, offset.y + 8);
	printf("HOLD");

	printxy(offset.x, offset.y + 16, "  ¡â    : Rotate");
	printxy(offset.x, offset.y + 17, "¢·  ¢¹  : Move");
	printxy(offset.x, offset.y + 18, "  ¡ä    : Move");
	printxy(offset.x, offset.y + 19, "P : Hard Down");
	printxy(offset.x, offset.y + 20, "H : Hold");
	setFontColor(RED);
	printxy(offset.x, offset.y + 22, "Run in Legacy Terminal or PowerShell");
	setFontColor(WHITE);
	printxy(offset.x, offset.y + 24, "By Ã¢Çù");
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
		gotoxy(BOARD_OFFSET.x + 1, y + BOARD_OFFSET.y + 1);
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
	Pos offset = { BOARD_OFFSET.x + 1, BOARD_OFFSET.y + 1 };
	Pos pos = block.pos;
	int blockNum = block.blockNum;
	int rotate = block.rotate;

	for (int i = 0; i < 4; i++) {
		int x_pos = pos.x + BLOCK[blockNum][rotate][i].x;
		int y_pos = pos.y + BLOCK[blockNum][rotate][i].y;

		gotoxy(x_pos + offset.x, y_pos + offset.y);
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

bool moveBlock(Board board, Block* block, int key) {
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

bool rotateBlock(Board board, Block* block) {
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

void setHoldStat(Block* hold, int stat) {
	hold->blockNum = stat;
}

int getHoldStat(Block hold) {
	return hold.blockNum;
}

void hardDown(Board board, Block* block, int blockType) {
	Block tmp = *block;

	setBlock(board, *block, ERASE);
	displayBlock(*block, ERASE);
	while (setBlock(board, *block, TEST))
		block->pos.y++;

	block->pos.y--;
	setBlock(board, *block, blockType);
	displayBlock(*block, blockType);
	if (blockType == SHADOW)
		setBlock(board, tmp, DRAW);
}

void refreshShadow(Board board, Block* block, Block* shadow) {
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

void displayHold(Block* hold, int blockType) {
	Pos offset = { HOLD_OFFSET.x + 2, HOLD_OFFSET.y + 1 };
	hold->pos = offset;
	displayBlock(*hold, blockType);
}

int dropBlock(Board board, Block block, Block* hold) {
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
				if (getHoldStat(*hold) == NOT_HOLDING) {
					holdBlock(board, block, hold);
					setBlock(board, shadow, ERASE);
					return PLAYING;
				} if (getHoldStat(*hold) == HOLDED) {
					break;
				}
				else {
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


void displayLine(const Board board, int col, const char* block) {
	Pos offset = { BOARD_OFFSET.x + 1, BOARD_OFFSET.y + 1 };
	gotoxy(offset.x, col + offset.y);
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

void removeLineAnimation(const Board board, int* cols, int numCol) {
	for (int t = 0; t < 5; t++) {
		for (int i = 0; i < numCol; i++) {
			int y = cols[i];
			if (t % 2 == 0)
				displayLine(board, y, "¡á");
			else
				displayLine(board, y, "  ");
		}
		Sleep(200);
	}
}

void removeLine(Board board, int* cols, int numCol) {
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
