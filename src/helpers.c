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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>

#include "helpers.h"
#include "log/log.h"

/*
	* Checks if the passed file path is a valid file and not a directory or a device file
	* Returns -> 0 (a valid file), 1 (a directory), 2 (a device file), 3 (doesn't exist)
*/
enum PathType GetPathInfo(char *file) {
	struct stat fileInfo;

	/* First check that the file exists and is readable. */
	if (access(file, R_OK) != 0)
		return PATH_ENOENT;

	/* If the thing exists, it may be neither a directory nor a device. */
	if (
		stat(file, &fileInfo) != -1 &&
		(
			S_ISDIR(fileInfo.st_mode)  ||
			S_ISCHR(fileInfo.st_mode)  ||
			S_ISBLK(fileInfo.st_mode))
		) {

		if (S_ISDIR(fileInfo.st_mode)) return PATH_DIR;
		return PATH_DEVICE;
	} else {
		return PATH_FILE;
	}
}

bool str_startswith(const char *prefix, const char *str) {
	return strncmp(prefix, str, strlen(prefix)) == 0;
}

char* _strdup(const char *str) {
	if (str == NULL) return NULL;

	int n = strlen(str) + 1;
	char* dup = malloc(n * sizeof(char));
	memset(dup, '\0', n * sizeof(char));

	if (dup) {
		memcpy(dup, str, n);
	}
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
