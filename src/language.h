#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <tre/tre.h>

#include "main.h"
#include "pattern.h"

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

typedef struct editorSyntax {
	char*      name;             // Language Name
	char**     extensions;       // Array Of File Extensions Like .c, .h, .cpp
	int        totalExtensions;  // Total Elements in "extensions"
	// KeywordsXX and totalKeywordsXX
	char**     keywords1;
	int        totalKeywords1;
	char**     keywords2;
	int        totalKeywords2;
	pattern_t**  patterns;
	int          totalPatterns;
	char*      singleline_comment_start; // Single-Line Comment Start Like "//"
	char*      multiline_comment_start;  // Multi-Line Comment Start Like "/*"
	char*      multiline_comment_end;    // Multi-Line Comment Start Like "*/"
	int        flags;                    // Flags For The Highlighter Like "HL_HIGHLIGHT_STRINGS" enables highlighting string
} language_t;

typedef struct {
	language_t** languages;
	int numOfLangs;
} language_arr_t;

language_t* LoadLanguage(const char* jsonText);
language_arr_t* LoadAllLanguages();
void FreeLanguage(language_t* L);
void FreeLanguageArr(language_arr_t* L_Arr);

#endif
