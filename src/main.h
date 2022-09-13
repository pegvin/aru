#ifndef MAIN_H
#define MAIN_H

#include "terminal.h"

// Defines

#if ARU_BUILD_STABLE == 0
	#define ARU_BUILD_TYPE "dev"
#else
	#define ARU_BUILD_TYPE "stable"
#endif

#ifndef ARU_VERSION
	#define ARU_VERSION "0.0.0"
#endif

#define ARU_TAB_STOP 4
#define ARU_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f)

typedef enum {
	HL_NORMAL = 0,
	HL_COMMENT,
	HL_MLCOMMENT,
	HL_KEYWORD1,
	HL_KEYWORD2,
	HL_STRING,
	HL_NUMBER,
	HL_MATCH
} syntax_color_t;

void die(const char *s);
void EditorSetStatusMessage(const char *fmt, ...);
void EditorRefreshScreen();
char* EditorPromptText(char *prompt, void (*callback)(char *, int));

#endif