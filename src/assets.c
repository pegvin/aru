#include "assets.h"
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
