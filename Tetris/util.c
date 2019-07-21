#include "util.h"

#ifdef LINUX

void gotoxy(int x, int y) {
	printf("\033[%d;%df", y, x * 2) ;
}

void hideCursor() {
	printf("\e[?25l");

	struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);           // 현재 터미널 설정 읽음
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);         // CANONICAL과 ECHO 끔
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);  // 터미널에 설정 입력
}

void setFontColor(int num) {
	printf("\033[0;%dm", num);
}

void sleepMilli(int ms) {
	usleep(ms * 1000);
}

bool _kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

char _getch() {
    char c;
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);           // 현재 터미널 설정 읽음
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);         // CANONICAL과 ECHO 끔
    newattr.c_cc[VMIN] = 1;                      // 최소 입력 문자 수를 1로 설정
    newattr.c_cc[VTIME] = 0;                     // 최소 읽기 대기 시간을 0으로 설정
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);  // 터미널에 설정 입력
    c = getchar();                               // 키보드 입력 읽음
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);  // 원래의 설정으로 복구
    return c;
}

#else

void gotoxy(int x, int y) {
	COORD pos = { 2 * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void hideCursor() {
	CONSOLE_CURSOR_INFO CursorInfo;
	CursorInfo.dwSize = 1;
	CursorInfo.bVisible = FALSE;

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);
}

void setFontColor(int num) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), num);
}

void sleepMilli(int ms) {
	Sleep(ms);
}

#endif

void printxy(int x, int y, const char* str) {
	gotoxy(x, y);
	printf("%s", str);
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
			case 119:
				return UP;
			case 97:
				return LEFT;
			case 100:
				return RIGHT;
			case 115:
				return DOWN;
			default:
				return key;
			}
		}
	}
	return NOTHING;
}
