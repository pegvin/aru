#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "helpers.h"

bool str_startswith(const char *prefix, const char *str) {
	return strncmp(prefix, str, strlen(prefix)) == 0;
}

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

/*
	Returns A String Containing The Number of Bytes in A readable format
	Converted From: https://stackoverflow.com/a/18650828/14516016
*/
char* formatBytes(int bytes) {
	static char newStr[512] = "";
	const int k = 1024;

	const int i = floor(log(bytes) / log(k));
	const float convertedSize = (bytes / pow(k, i));
	switch (i) {
		case 0:
			snprintf(newStr, 512, "%0.2f Bytes", convertedSize);
			break;
		case 1:
			snprintf(newStr, 512, "%0.2f KB", convertedSize);
			break;
		case 2:
			snprintf(newStr, 512, "%0.2f MB", convertedSize);
			break;
		case 3:
			snprintf(newStr, 512, "%0.2f GB", convertedSize);
			break;
		case 4:
			snprintf(newStr, 512, "%0.2f TB", convertedSize);
			break;
		case 5:
			snprintf(newStr, 512, "%0.2f PB", convertedSize);
			break;
		case 6:
			snprintf(newStr, 512, "%0.2f EB", convertedSize);
			break;
		case 7:
			snprintf(newStr, 512, "%0.2f ZB", convertedSize);
			break;
		case 8:
			snprintf(newStr, 512, "%0.2f YB", convertedSize);
			break;
	}

	return newStr;
}
