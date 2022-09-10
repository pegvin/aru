#include <stdlib.h>
#include <string.h>

#include "theme.h"
#include "ini/ini.h"
#include "colors.h"

void _set_color(const char* str, int* color) {
	if (str == NULL)
		(*color) = FG_WHITE;

	if (strncmp(str, "FG_BLACK", 8) == 0) {
		(*color) = FG_BLACK;
	} else if (strncmp(str, "FG_RED", 6) == 0) {
		(*color) = FG_RED;
	} else if (strncmp(str, "FG_GREEN", 8) == 0) {
		(*color) = FG_GREEN;
	} else if (strncmp(str, "FG_YELLOW", 9) == 0) {
		(*color) = FG_YELLOW;
	} else if (strncmp(str, "FG_BLUE", 7) == 0) {
		(*color) = FG_BLUE;
	} else if (strncmp(str, "FG_MAGENTA", 10) == 0) {
		(*color) = FG_MAGENTA;
	} else if (strncmp(str, "FG_CYAN", 7) == 0) {
		(*color) = FG_CYAN;
	} else if (strncmp(str, "FG_WHITE", 8) == 0) {
		(*color) = FG_WHITE;
	} else {
		(*color) = FG_WHITE;
	}
}

theme_t* ThemeLoadFrom(const char* iniText) {
	theme_t* t = malloc(sizeof(theme_t));
	ini_t* themeConf = ini_load_txt(iniText);

	if (iniText == NULL) {
		t->COMMENT = FG_WHITE;
		t->MLCOMMENT = FG_WHITE;
		t->KEYWORD1 = FG_WHITE;
		t->KEYWORD2 = FG_WHITE;
		t->STRING = FG_WHITE;
		t->NUMBER = FG_WHITE;
		t->MATCH = FG_WHITE;
		t->COMMENT = FG_WHITE;
		t->DEFAULT = FG_WHITE;
		return t;
	}

	const char* _COMMENT   = ini_get(themeConf, "theme", "COMMENT");
	_set_color(_COMMENT, &t->COMMENT);

	const char* _MLCOMMENT = ini_get(themeConf, "theme", "MLCOMMENT");
	_set_color(_MLCOMMENT, &t->MLCOMMENT);

	const char* _KEYWORD1  = ini_get(themeConf, "theme", "KEYWORD1");
	_set_color(_KEYWORD1, &t->KEYWORD1);

	const char* _KEYWORD2  = ini_get(themeConf, "theme", "KEYWORD2");
	_set_color(_KEYWORD2, &t->KEYWORD2);

	const char* _STRING    = ini_get(themeConf, "theme", "STRING");
	_set_color(_STRING, &t->STRING);

	const char* _NUMBER    = ini_get(themeConf, "theme", "NUMBER");
	_set_color(_NUMBER, &t->NUMBER);

	const char* _MATCH     = ini_get(themeConf, "theme", "MATCH");
	_set_color(_MATCH, &t->MATCH);

	const char* _DEFAULT     = ini_get(themeConf, "theme", "DEFAULT");
	_set_color(_DEFAULT, &t->DEFAULT);

	ini_free(themeConf);
	themeConf = NULL;
	return t;
}

void FreeTheme(theme_t* t) {
	if (t != NULL)
		free(t);
}
