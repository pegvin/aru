#include "pattern.h"
#include <stdlib.h>

pattern_t* LoadPattern(const char* regexStr) {
	pattern_t* p = NULL;

	if (regexStr != NULL) {
		regex_t* r = malloc(sizeof(regex_t));
		int result = tre_regcomp(r, regexStr, REG_EXTENDED);
		if (result == REG_OK) {
			p = malloc(sizeof(pattern_t));
			p->regex = r;
			p->color = 36;
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
