#pragma once

#define LINUX

#include <stdio.h>

#ifdef LINUX

#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdbool.h>
#include <fcntl.h>

#define RED   31
#define WHITE 1
#define GRAY  2

#define CLEAR_CONSOLE "clear"

#else

#include <Windows.h>
#include <conio.h>

#define RED   4
#define WHITE 7
#define GRAY  8

#define CLEAR_CONSOLE "cls"

#endif

typedef struct { int x, y; } Pos;

enum { NOTHING = -1, LEFT, RIGHT, UP, DOWN, ENTER };

void gotoxy(int x, int y);
void printxy(int x, int y, const char* str);
void hideCursor();
void setFontColor(int num);
int keyboard();
void sleepMilli(int ms);
