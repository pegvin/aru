#ifndef LANGUAGE_H
#define LANGUAGE_H

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

struct editorSyntax {
	char*  filetype;      // Language Name Like C
	char** filematch;     // Array Of File Extensions Like .c, .h, .cpp
	int    totalMatches;  // Total Elements in "filematch"
	char** keywords;      // Array Of Language Keywords
	int    totalKeywords; // Total Elements in "keywords"
	char*  singleline_comment_start; // Single-Line Comment Start Like "//"
	char*  multiline_comment_start;  // Multi-Line Comment Start Like "/*"
	char*  multiline_comment_end;    // Multi-Line Comment Start Like "*/"
	int    flags;                    // Flags For The Highlighter Like "HL_HIGHLIGHT_STRINGS" enables highlighting string
};

typedef struct editorSyntax language_t;

typedef struct {
	language_t** languages;
	int numOfLangs;
} language_arr_t;

language_t* LoadLanguage(const char* jsonText);
language_arr_t* LoadAllLanguages();
void FreeLanguage(language_t* L);
void FreeLanguageArr(language_arr_t* L_Arr);

#endif