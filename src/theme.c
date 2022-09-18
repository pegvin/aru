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
#include <json-c/json.h>

#include "theme.h"
#include "colors.h"
#include "helpers.h"

void _set_color(char* name, int value, theme_t* t) {
	int* color = NULL;

	if (strncmp(name, "comment", 7) == 0) {
		color = &t->COMMENT;
	} else if (strncmp(name, "mlcomment", 9) == 0) {
		color = &t->MLCOMMENT;
	} else if (strncmp(name, "keyword1", 8) == 0) {
		color = &t->KEYWORD1;
	} else if (strncmp(name, "keyword2", 8) == 0) {
		color = &t->KEYWORD2;
	} else if (strncmp(name, "string", 6) == 0) {
		color = &t->STRING;
	} else if (strncmp(name, "number", 6) == 0) {
		color = &t->NUMBER;
	} else if (strncmp(name, "match", 5) == 0) {
		color = &t->MATCH;
	} else if (strncmp(name, "default", 7) == 0) {
		color = &t->DEFAULT;
	} else {
		color = &t->DEFAULT;
	}

	(*color) = value;
}

theme_t* ThemeLoadFrom(const char* jsonText) {
	struct json_object* ParsedJSON;
	struct json_object* name;
	struct json_object* _colors;

	ParsedJSON = json_tokener_parse(jsonText);
	json_object_object_get_ex(ParsedJSON, "name", &name);
	json_object_object_get_ex(ParsedJSON, "colors", &_colors);

	printf("Loading %s theme...\n", json_object_get_string(name));

	theme_t* t = malloc(sizeof(theme_t));

	{
		int _col = -1;
		if (jsonText == NULL) _col = FG_WHITE;

		t->DEFAULT = _col;
		t->COMMENT = _col;
		t->MLCOMMENT = _col;
		t->KEYWORD1 = _col;
		t->KEYWORD2 = _col;
		t->STRING = _col;
		t->NUMBER = _col;
		t->MATCH = _col;
	}

	if (jsonText == NULL)
		return t;

	enum json_type type;
	json_object_object_foreach(_colors, key, val) {
		type = json_object_get_type(val);
		switch (type) {
			case json_type_int: {
				int parsedVal = json_object_get_int(val);
				strLower(key, strlen(key));
				_set_color(key, parsedVal, t);
				break;
			}
			case json_type_string:
			case json_type_null:
			case json_type_boolean:
			case json_type_double:
			case json_type_object:
			case json_type_array:
				break;
		}
	}

	while (json_object_put(ParsedJSON) != 1) {}
	ParsedJSON = NULL;
	name = NULL;
	_colors = NULL;

	return t;
}

void FreeTheme(theme_t* t) {
	if (t != NULL)
		free(t);
}
