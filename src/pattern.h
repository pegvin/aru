#ifndef PATTERN_H
#define PATTERN_H

#include <tre/tre.h>
#include "main.h"

typedef struct {
	regex_t* regex;
	syntax_color_t color;
} pattern_t;

pattern_t* LoadPattern(const char* regexStr);
void FreePattern(pattern_t* p);

#endif