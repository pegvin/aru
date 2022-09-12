#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#include "theme.h"
#include "colors.h"
#include "helpers.h"

#if IS_DEBUG
void printColor(int color) {
	switch (color) {
		case FG_BLACK:
			printf("FG_BLACK\n");
			break;
		case FG_RED:
			printf("FG_RED\n");
			break;
		case FG_GREEN:
			printf("FG_GREEN\n");
			break;
		case FG_YELLOW:
			printf("FG_YELLOW\n");
			break;
		case FG_BLUE:
			printf("FG_BLUE\n");
			break;
		case FG_MAGENTA:
			printf("FG_MAGENTA\n");
			break;
		case FG_CYAN:
			printf("FG_CYAN\n");
			break;
		case FG_WHITE:
			printf("FG_WHITE\n");
			break;
		default:
			printf("Unknown Color %d\n", color);
			break;
	}
}

void printTheme(theme_t* t) {
	printf("DEFAULT - ");
	printColor(t->DEFAULT);
	printf("COMMENT - ");
	printColor(t->COMMENT);
	printf("MLCOMMENT - ");
	printColor(t->MLCOMMENT);
	printf("KEYWORD1 - ");
	printColor(t->KEYWORD1);
	printf("KEYWORD2 - ");
	printColor(t->KEYWORD2);
	printf("STRING - ");
	printColor(t->STRING);
	printf("NUMBER - ");
	printColor(t->NUMBER);
	printf("MATCH - ");
	printColor(t->MATCH);
}
#endif // IS_DEBUG

void _set_color(char* name, char* value, theme_t* t) {
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

	if (value == NULL) {
		(*color) = FG_WHITE;
		return;
	}

	if (strncmp(value, "fg_black", 8) == 0) {
		(*color) = FG_BLACK;
	} else if (strncmp(value, "fg_red", 6) == 0) {
		(*color) = FG_RED;
	} else if (strncmp(value, "fg_green", 8) == 0) {
		(*color) = FG_GREEN;
	} else if (strncmp(value, "fg_yellow", 9) == 0) {
		(*color) = FG_YELLOW;
	} else if (strncmp(value, "fg_blue", 7) == 0) {
		(*color) = FG_BLUE;
	} else if (strncmp(value, "fg_magenta", 10) == 0) {
		(*color) = FG_MAGENTA;
	} else if (strncmp(value, "fg_cyan", 7) == 0) {
		(*color) = FG_CYAN;
	} else if (strncmp(value, "fg_white", 8) == 0) {
		(*color) = FG_WHITE;
	} else {
		(*color) = FG_WHITE;
	}
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
			case json_type_string: {
				char* parsedVal = (char*)json_object_get_string(val);
				strLower(key, strlen(key));
				strLower(parsedVal, strlen(parsedVal));
				// printf("_set_color(\"%s\", \"%s\", %p);\n", key, parsedVal, (void*)t);
				_set_color(key, parsedVal, t);
				break;
			}
			case json_type_null:
			case json_type_boolean:
			case json_type_double:
			case json_type_int:
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
