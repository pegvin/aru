#ifndef THEME_H
#define THEME_H

typedef struct {
	int COMMENT;
	int MLCOMMENT;
	int KEYWORD1;
	int KEYWORD2;
	int STRING;
	int NUMBER;
	int MATCH;
	int DEFAULT;
} theme_t;

theme_t* ThemeLoadFrom(const char* jsonText);
void FreeTheme(theme_t* t);

#endif