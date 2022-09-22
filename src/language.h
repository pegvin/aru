/* BSD 2-Clause License
 *
 * Copyright (c) 2022, Aditya Mishra
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <tre/tre.h>

#include "main.h"
#include "pattern.h"

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

typedef struct editorSyntax {
	char*      name;             // Language Name
    pattern_t* filePattern;      // File Extension Regex
	// char**     extensions;       // Array Of File Extensions Like .c, .h, .cpp
	// int        totalExtensions;  // Total Elements in "extensions"
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
