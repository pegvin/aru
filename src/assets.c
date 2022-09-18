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

#include "assets.h"
#include "helpers.h"
#include <stdio.h>

typedef struct {
	const char      *path;
	int             size;
	const void      *data __attribute__((aligned(4)));
} asset_t;

static asset_t ASSETS[] = {
	#include "assets/languages.inl"
	#include "assets/themes.inl"
};

const void* AssetsGet(const char *filePath, int *size) {
	int i;
	if (str_startswith("asset://", filePath) == true) filePath += 8; // Skip asset://
	for (i = 0; ASSETS[i].path; i++) {
		if (strcmp(ASSETS[i].path, filePath) == 0) {
			if (size) *size = ASSETS[i].size;
			return ASSETS[i].data;
		}
	}
	return NULL;
}

int AssetsList(const char* directoryPath, int (*callback)(int i, const char *path)) {
	int i, j = 0;
	for (i = 0; ASSETS[i].path; i++) {
		if (str_startswith(directoryPath, ASSETS[i].path)) {
			if (callback != NULL) {
				if (callback(j, ASSETS[i].path) == 0)
					j++;
			}
		}
	}
	return j;
}
