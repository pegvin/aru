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

#include <string.h>

#include "language.h"
#include "log/log.h"
#include "ini/ini.h"
#include "assets.h"
#include "helpers.h"

language_t* LoadLanguage(const char* jsonText) {
	language_t* L = NULL;

	ini_t* IniConfig = ini_load_txt(jsonText);
	if (IniConfig == NULL) return NULL;

	const char* name = ini_get(IniConfig, "aru.theme", "name");
	const char* filePattern = ini_get(IniConfig, "aru.theme", "filePattern");
	const char* sLineComment = ini_get(IniConfig, "aru.theme", "singleLineComment");
	const char* mCommentStart = ini_get(IniConfig, "aru.theme", "multiLineCommentStart");
	const char* mCommentEnd = ini_get(IniConfig, "aru.theme", "multiLineCommentEnd");
	const char* totalPatterns = ini_get(IniConfig, "aru.theme", "totalPatterns");
	const char* keywords1 = ini_get(IniConfig, "aru.theme", "keywords1");
	const char* keywords2 = ini_get(IniConfig, "aru.theme", "keywords2");

	if (filePattern == NULL) goto onFail;

	pattern_t* _filePat = LoadPattern(filePattern, "default");
	if (_filePat == NULL) goto onFail;

	// log_info("%s", name);
	// log_info("%s", sLineComment);
	// log_info("%s", mCommentStart);
	// log_info("%s", mCommentEnd);
	// log_info("%s", totalPatterns);
	// log_info("%s", keywords1);
	// log_info("%s", keywords2);

	L = malloc(sizeof(language_t));
	L->name = _strdup(name);
	L->filePattern = _filePat;
	L->singleline_comment_start = _strdup(sLineComment);
	L->multiline_comment_start = _strdup(mCommentStart);
	L->multiline_comment_end = _strdup(mCommentEnd);
	L->flags = HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS;
	L->keywords1 = NULL;
	L->keywords2 = NULL;
	L->patterns = NULL;
	L->totalKeywords1 = 0;
	L->totalKeywords2 = 0;
	L->totalPatterns = 0;

	if (totalPatterns != NULL) {
		int totalPatternsInt = atoi(totalPatterns);
		if (totalPatternsInt > 0) {
			L->patterns = NULL;
			L->patterns = malloc(sizeof(pattern_t*) * totalPatternsInt);
			L->totalPatterns = 0;

			#define _SECTION_STR_MAX 1024
			char sectionStr[_SECTION_STR_MAX] = ""; // Holds Pattern Section Name (like: aru.pattern1)
			for (int i = 0; i < totalPatternsInt; ++i) {
				memset(sectionStr, '\0', _SECTION_STR_MAX);
				snprintf(sectionStr, _SECTION_STR_MAX, "aru.pattern%d", i);
				const char* regex = ini_get(IniConfig, sectionStr, "pattern");
				const char* type = ini_get(IniConfig, sectionStr, "type");
				if (regex == NULL) continue;

				pattern_t* p = LoadPattern(regex, type == NULL ? "default" : type);
				if (p == NULL) continue;

				L->patterns[i] = p;
				L->totalPatterns++;
			}

			#undef _SECTION_STR_MAX
		}
	}

	if (keywords1 != NULL) {
		char* copy = _strdup(keywords1);
		L->totalKeywords1 = 0;
		char* token = NULL;

		// Calculate Total Keywords
		token = strtok(copy, " ");
		while (token != NULL) {
			L->totalKeywords1++;
			token = strtok(NULL, " ");
		}

		// Allocate Memory & Copy Strings
		if (L->totalKeywords1 > 0) {
			free(copy);
			copy = _strdup(keywords1);
			L->keywords1 = malloc(sizeof(char*) * L->totalKeywords1);
			token = strtok(copy, " ");
			int i = 0;
			while (token != NULL) {
				L->keywords1[i] = _strdup(token);
				token = strtok(NULL, " ");
				i++;
			}
		}

		free(copy);
		copy = NULL;
	}

	if (keywords2 != NULL) {
		char* copy = _strdup(keywords2);
		L->totalKeywords2 = 0;
		char* token = NULL;

		// Calculate Total Keywords
		token = strtok(copy, " ");
		while (token != NULL) {
			L->totalKeywords2++;
			token = strtok(NULL, " ");
		}

		// Allocate Memory & Copy Strings
		if (L->totalKeywords2 > 0) {
			free(copy);
			copy = _strdup(keywords2);
			L->keywords2 = malloc(sizeof(char*) * L->totalKeywords2);
			token = strtok(copy, " ");
			int i = 0;
			while (token != NULL) {
				L->keywords2[i] = _strdup(token);
				token = strtok(NULL, " ");
				i++;
			}
		}

		free(copy);
		copy = NULL;
	}

onFail:
	ini_free(IniConfig);
	IniConfig = NULL;

	return L;
}

language_arr_t* LoadAllLanguages() {
	language_arr_t* L_Arr = malloc(sizeof(language_arr_t));
	L_Arr->numOfLangs = 1;
	L_Arr->languages = malloc(sizeof(language_t*) * L_Arr->numOfLangs);
	L_Arr->languages[0] = LoadLanguage(AssetsGet("data/languages/c.ini", NULL));

	return L_Arr;
}

void FreeLanguageArr(language_arr_t* L_Arr) {
	if (L_Arr == NULL) return;

	for (int i = 0; i < L_Arr->numOfLangs; ++i) {
		if (L_Arr->languages[i] != NULL) {
			FreeLanguage(L_Arr->languages[i]);
			L_Arr->languages[i] = NULL;
		}
	}

	free(L_Arr->languages);
	L_Arr->languages = NULL;
	L_Arr->numOfLangs = -1;
	free(L_Arr);
}

void FreeLanguage(language_t* L) {
	if (L == NULL) return;

	FreePattern(L->filePattern);
	L->filePattern = NULL;

	if (L->keywords1 != NULL) {
		for (int i = 0; i < L->totalKeywords1; ++i) {
			if (L->keywords1[i] != NULL) {
				free(L->keywords1[i]);
				L->keywords1[i] = NULL;
			}
		}
		free(L->keywords1);
		L->keywords1 = NULL;
	}

	if (L->keywords2 != NULL) {
		for (int i = 0; i < L->totalKeywords2; ++i) {
			if (L->keywords2[i] != NULL) {
				free(L->keywords2[i]);
				L->keywords2[i] = NULL;
			}
		}
		free(L->keywords2);
		L->keywords2 = NULL;
	}

	if (L->patterns != NULL) {
		for (int i = 0; i < L->totalPatterns; ++i) {
			if (L->patterns[i] != NULL) {
				FreePattern(L->patterns[i]);
				L->patterns[i] = NULL;
			}
		}
		free(L->patterns);
		L->patterns = NULL;
	}

	if (L->name != NULL) {
		free(L->name);
		L->name = NULL;
	}

	if (L->singleline_comment_start != NULL) {
		free(L->singleline_comment_start);
		L->singleline_comment_start = NULL;
	}

	if (L->multiline_comment_start != NULL) {
		free(L->multiline_comment_start);
		L->multiline_comment_start = NULL;
	}

	if (L->multiline_comment_end != NULL) {
		free(L->multiline_comment_end);
		L->multiline_comment_end = NULL;
	}

	L->flags = 0;
	L->totalKeywords1 = -1;
	free(L);
}


