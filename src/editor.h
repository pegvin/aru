#ifndef EDITOR_H
#define EDITOR_H 1

#include "theme.h"
#include "language.h"

typedef struct {
	int idx;
	int size;
	int rsize;
	char* chars;
	char* render;
	unsigned char* hl;
	int hl_open_comment;
} erow;

typedef struct {
	int cx, cy;
	int rx;
	int rowoff;
	int coloff;
	int screenrows;
	int screencols;
	int numrows;
	erow* row;
	int dirty;
	char* filename;
	char statusmsg[80];
	time_t statusmsg_time;
	theme_t* theme;
	language_t* syntax;
} editor_t;

#endif
