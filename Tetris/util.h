#pragma once

#include <stdio.h>
#include <Windows.h>
#include <conio.h>

#define BLACK 0
#define RED   4
#define WHITE 7
#define GRAY  8

typedef struct { int x, y; } Pos;

enum { NOTHING = -1, LEFT, RIGHT, UP, DOWN, ENTER };

void gotoxy(int x, int y);
void printxy(int x, int y, const char* str);
void hideCursor();
void setFontColor(int num);
int keyboard();