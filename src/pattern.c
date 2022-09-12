#include <stdlib.h>
#include <string.h>
#include "colors.h"
#include "pattern.h"
#include "helpers.h"

pattern_t* LoadPattern(const char* regexStr, const char* colorStr) {
	pattern_t* p = NULL;

	if (regexStr != NULL) {
		regex_t* r = malloc(sizeof(regex_t));
		int result = tre_regcomp(r, regexStr, REG_EXTENDED);
		if (result == REG_OK) {
			p = malloc(sizeof(pattern_t));
			p->regex = r;
			if (colorStr != NULL) {
				strLower(colorStr, strlen(colorStr));
				if (strncmp(colorStr, "comment", 7) == 0) {
					p->color = HL_COMMENT;
				} else if (strncmp(colorStr, "mlcomment", 9) == 0) {
					p->color = HL_MLCOMMENT;
				} else if (strncmp(colorStr, "keyword1", 8) == 0) {
					p->color = HL_KEYWORD1;
				} else if (strncmp(colorStr, "keyword2", 8) == 0) {
					p->color = HL_KEYWORD2;
				} else if (strncmp(colorStr, "string", 6) == 0) {
					p->color = HL_STRING;
				} else if (strncmp(colorStr, "number", 6) == 0) {
					p->color = HL_NUMBER;
				} else if (strncmp(colorStr, "match", 5) == 0) {
					p->color = HL_MATCH;
				} else if (strncmp(colorStr, "default", 7) == 0) {
					p->color = HL_NORMAL;
				} else {
					p->color = HL_NORMAL;
				}
			} else {
				p->color = HL_NORMAL;
			}
		} else {
			tre_regfree(r);
			r = NULL;
		}
	}

	return p;
}

void FreePattern(pattern_t* p) {
	if (p != NULL) {
		tre_regfree(p->regex);
		p->regex = NULL;
		p->color = 0;
		free(p);
	}
}
