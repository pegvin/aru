#ifndef TERMINAL_H
#define TERMINAL_H 1

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "editor.h"

// Structs & Enums
typedef enum {
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN
} termkey_t;

// Uses Simple Escape Codes To Get The Cursor Positon
int TermGetCursorPos(int *rows, int *cols);

// Uses Simple Escape Codes To Get The Window Size in Rows & Columns & Not Pixels
int TermGetWinSize(int *rows, int *cols);

void TermEnableRawMode();
void TermDisableRawMode();
int TermReadKey();
void TermSwitchToAlternativeScreen();
void TermSwitchToMainScreen();

#endif