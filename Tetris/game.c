#include "game.h"

void play() {
	init();
	Board board = { 0, };

	int level = 1;
	int score = 0;

	Block curr;
	Block next = newBlock();
	Block hold;
	setHoldStat(&hold, NOT_HOLDING);

	srand((unsigned int)time(NULL));
	do {
		checkLine(board, &score, &level);
		displayBoard(board);

		curr = next;
		displayNextBlock(next, ERASE);
		next = newBlock();
		displayNextBlock(next, DRAW);

		if (getHoldStat(hold) == HOLDED)
			setHoldStat(&hold, NOT_HOLDING);
	} while (dropBlock(board, curr, &hold, level) == PLAYING);
}

Block newBlock() {
	Block new;

	new.pos.x = NUM_ROW / 2 - 1;
	new.pos.y = 1;
	new.blockNum = rand() % NUM_BLOCKS;
	new.rotate = rand() % NUM_ROTATE;

	return new;
}


// init and display
void init() {
	system(CLEAR_CONSOLE);
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

	printxy(offset.x, offset.y + 16, "  △    : Rotate");
	printxy(offset.x, offset.y + 17, "◁   ▷  : Move");
	printxy(offset.x, offset.y + 18, "  ▽    : Move");
	printxy(offset.x, offset.y + 19, "P : Hard Down");
	printxy(offset.x, offset.y + 20, "H : Hold");
	printxy(offset.x, offset.y + 22, "Level : 1");
	printxy(offset.x, offset.y + 23, "Score : 0");
	setFontColor(RED);
	printxy(offset.x, offset.y + 25, "Run in Legacy Terminal or PowerShell");
	setFontColor(WHITE);
	printxy(offset.x, offset.y + 26, "By 창협");
}

void drawOutLine(int width, int height, Pos offset) {
	gotoxy(offset.x, offset.y);
	for (int x = 0; x < width; x++)
		printf("■ ");
	for (int y = 1; y < height - 1; y++) {
		gotoxy(offset.x, y + offset.y);
		printf("■ ");
		for (int x = 0; x < width - 2; x++)
			printf("  ");
		printf("■ \n");
	}
	gotoxy(offset.x, offset.y + height - 1);
	for (int x = 0; x < width; x++)
		printf("■ ");
}

void displayBoard(const Board board) {
	for (int y = 0; y < NUM_COL; y++) {
		gotoxy(BOARD_OFFSET.x + 1, y + BOARD_OFFSET.y + 1);
		for (int x = 0; x < NUM_ROW; x++) {
			if (board[y][x] == 1)
				printf("■ ");
			else if (board[y][x] == 2)
				printf("□ ");
			else
				printf("  ");
		}
	}
}

void displayBlock(Block block, int blockType, Pos offset) {
	Pos pos = block.pos;
	int blockNum = block.blockNum;
	int rotate = block.rotate;

	for (int i = 0; i < 4; i++) {
		int x_pos = pos.x + BLOCK[blockNum][rotate][i].x;
		int y_pos = pos.y + BLOCK[blockNum][rotate][i].y;

		gotoxy(x_pos + offset.x, y_pos + offset.y);
		if (blockType == DRAW)
			printf("■ ");
		else if (blockType == SHADOW)
			printf("□ ");
		else
			printf("  ");
	}
}

void displayNextBlock(Block next, int blockType) {
	next.pos.x = 3;
	next.pos.y = 2;
	displayBlock(next, blockType, NEXT_OFFSET);
}


//Moving Block
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
	displayBlock(*block, ERASE, INBOARD_OFFSET);
	block->pos.x += xMove;
	block->pos.y += yMove;
	if (!setBlock(board, *block, TEST)) {
		block->pos.x -= xMove;
		block->pos.y -= yMove;
		setBlock(board, *block, DRAW);
		displayBlock(*block, DRAW, INBOARD_OFFSET);

		return false;
	}
	setBlock(board, *block, DRAW);

	return true;
}

bool rotateBlock(Board board, Block* block) {
	setBlock(board, *block, ERASE);
	displayBlock(*block, ERASE, INBOARD_OFFSET);
	block->rotate++;
	block->rotate %= 4;

	if (!setBlock(board, *block, TEST)) {
		block->rotate--;
		block->rotate %= 4;
		setBlock(board, *block, DRAW);
		displayBlock(*block, DRAW, INBOARD_OFFSET);

		return false;
	}
	setBlock(board, *block, DRAW);
	displayBlock(*block, DRAW, INBOARD_OFFSET);

	return true;
}

void hardDown(Board board, Block* block, int blockType) {
	Block tmp = *block;

	setBlock(board, *block, ERASE);
	displayBlock(*block, ERASE, INBOARD_OFFSET);
	while (setBlock(board, *block, TEST))
		block->pos.y++;

	block->pos.y--;
	setBlock(board, *block, blockType);
	displayBlock(*block, blockType, INBOARD_OFFSET);
	if (blockType == SHADOW)
		setBlock(board, tmp, DRAW);
}

void refreshShadow(Board board, Block* block, Block* shadow) {
	setBlock(board, *shadow, ERASE);
	displayBlock(*shadow, ERASE, INBOARD_OFFSET);
	*shadow = *block;
	hardDown(board, shadow, SHADOW);
	displayBlock(*block, DRAW, INBOARD_OFFSET);
}


// Holding Block
void holdBlock(Board board, Block block, Block* hold) {
	setBlock(board, block, ERASE);
	displayBlock(block, ERASE, INBOARD_OFFSET);
	*hold = block;
	displayHold(hold, DRAW);
}

void getBlock(Board board, Block* block, Block* hold) {
	Pos curPos = block->pos;

	setBlock(board, *block, ERASE);
	displayBlock(*block, ERASE, INBOARD_OFFSET);
	*block = *hold;
	block->pos = curPos;
	displayHold(hold, ERASE);
	hold->blockNum = -2;
}

void displayHold(Block* hold, int blockType) {
	hold->pos.x = 3;
	hold->pos.y = 2;
	displayBlock(*hold, blockType, HOLD_OFFSET);
}

void setHoldStat(Block* hold, int stat) {
	hold->blockNum = stat;
}

int getHoldStat(Block hold) {
	return hold.blockNum;
}

// Game Process
int dropBlock(Board board, Block block, Block* hold, int level) {
	if (!setBlock(board, block, DRAW))
		return GAME_OVER;

	Block shadow = block;
	refreshShadow(board, &block, &shadow);

	do {
		displayBlock(block, DRAW, INBOARD_OFFSET);
		int speed = 100 - (level - 1) * 4;
		for (int i = 0; i < speed; i++) {
			sleepMilli(5);
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

void displayLine(int col, const char* block) {
	Pos offset = { BOARD_OFFSET.x + 1, BOARD_OFFSET.y + 1 };
	gotoxy(offset.x, col + offset.y);
	for (int x = 0; x < NUM_ROW; x++) {
		printf("%s", block);
	}
	fflush(stdout);
}

void pullLine(Board board, int from) {
	for (int y = from; y > 0; y--) {
		for (int x = 0; x < NUM_ROW; x++)
			board[y][x] = board[y - 1][x];
	}
}

void removeLineAnimation(int* cols, int numCol) {
	for (int t = 0; t < 5; t++) {
		for (int i = 0; i < numCol; i++) {
			int y = cols[i];
			if (t % 2 == 0)
				displayLine(y, "■ ");
			else
				displayLine(y, "  ");
		}
		printf("\n");
		sleepMilli(200);
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

void checkLine(Board board, int* score, int *level) {
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
	manageGameData(level, score, lineCount);
	if (lineCount != 0) {
		removeLine(board, removeCols, lineCount);
		removeLineAnimation(removeCols, lineCount);
	}
}

void manageGameData(int* level, int *score, int lineCount) {
	Pos offset = { NUM_ROW + 4 + BOARD_OFFSET.x, BOARD_OFFSET.y };

	*score += (10 + *level) * lineCount * lineCount;
	if ((*level * *level) <= *score / 40)
		(*level) += 1;

	printxy(offset.x, offset.y + 22, "Level : ");
	printf("%d", *level);
	printxy(offset.x, offset.y + 23, "Score : ");
	printf("%d", *score);
}
