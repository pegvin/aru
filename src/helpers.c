#include "helpers.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* _strdup(const char *str) {
	if (str == NULL) return NULL;

	int n = strlen(str) + 1;
	char *dup = malloc(n);
	if(dup) strcpy(dup, str);
	return dup;
}

void strLower(char* str, int len) {
	for (int i = 0; i < len; ++i) {
		str[i] = tolower(str[i]);
	}
}
