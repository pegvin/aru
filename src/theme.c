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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "theme.h"
#include "colors.h"
#include "helpers.h"
#include "ini/ini.h"
#include "log/log.h"

theme_t* ThemeLoadFrom(const char* jsonText) {
	ini_t* IniConfig = ini_load_txt(jsonText);

	theme_t* t = malloc(sizeof(theme_t));

	{
		int _col = -1;
		if (IniConfig == NULL) _col = FG_WHITE;

		t->DEFAULT = _col;
		t->COMMENT = _col;
		t->MLCOMMENT = _col;
		t->KEYWORD1 = _col;
		t->KEYWORD2 = _col;
		t->STRING = _col;
		t->NUMBER = _col;
		t->MATCH = _col;
	}

	if (IniConfig != NULL) {
		const char* name = ini_get(IniConfig, "aru.theme", "name");
		if (name != NULL)
			log_info("Loading %s theme...\n", name);

		const char* comment = ini_get(IniConfig, "aru.theme", "comment");
		const char* mlcomment = ini_get(IniConfig, "aru.theme", "mlcomment");
		const char* keyword1 = ini_get(IniConfig, "aru.theme", "keyword1");
		const char* keyword2 = ini_get(IniConfig, "aru.theme", "keyword2");
		const char* string = ini_get(IniConfig, "aru.theme", "string");
		const char* number = ini_get(IniConfig, "aru.theme", "number");
		const char* match = ini_get(IniConfig, "aru.theme", "match");
		const char* _default = ini_get(IniConfig, "aru.theme", "default");

		if (_default != NULL)  { t->DEFAULT = atoi(_default); }
		if (comment != NULL)   { t->COMMENT = atoi(comment); }
		if (mlcomment != NULL) { t->MLCOMMENT = atoi(mlcomment); }
		if (keyword1 != NULL)  { t->KEYWORD2 = atoi(keyword1); }
		if (keyword2 != NULL)  { t->KEYWORD1 = atoi(keyword2); }
		if (string != NULL)    { t->STRING = atoi(string); }
		if (number != NULL)    { t->NUMBER = atoi(number); }
		if (match != NULL)     { t->MATCH = atoi(match); }
	}

	if (IniConfig != NULL) ini_free(IniConfig);
	IniConfig = NULL;
	return t;
}

void FreeTheme(theme_t* t) {
	if (t != NULL)
		free(t);
}
