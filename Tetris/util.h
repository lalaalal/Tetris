#pragma once

#include <Windows.h>
#include <conio.h>

#define BLACK 0
#define RED   4
#define WHITE 7
#define GRAY  8

typedef struct { int x, y; } Pos;

enum { NOTHING = -1, LEFT, RIGHT, UP, DOWN, ENTER };

void gotoxy(int x, int y) {
	COORD pos = { 2 * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void hideCursor() {
	CONSOLE_CURSOR_INFO CursorInfo;
	CursorInfo.dwSize   = 1;
	CursorInfo.bVisible = FALSE;

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);
}

void setFontColor(int num) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), num);
}

int keyboard() {
	if (_kbhit()) {
		int key = _getch();
		if (key == 224 || key == 0) {
			key = _getch();
			switch (key) {
			case 72:
				return UP;
			case 75:
				return LEFT;
			case 77:
				return RIGHT;
			case 80:
				return DOWN;
			default:
				return -1;
			}
		}
		else {
			switch (key) {
			case 13:
				return ENTER;
			default:
				return key;
			}
		}
	}
	return NOTHING;
}