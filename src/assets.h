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

#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// All the assets are saved in binary directly in the code, using tool/create_assets.py.

/*
	Function: AssetsGet()
	Arguments:
		filePath: ex - data/fonts/Montserrat-Bold.ttf
		size: size of the data will be stored in this variable
*/
const void* AssetsGet(const char *filePath, int *size);

/*
	Function: AssetsList()
	Description: List all the assets in a given asset dir,
				 returns the number of assets listed, calls
				 the callback function on every item.
				 if the callback doesn't return 0, the asset
				 is skipped.
	Arguments:
		directoryPath: directory to list items of
		callback: function to call on every item
*/
int AssetsList(const char* directoryPath, int (*callback)(int i, const char *path));

#ifdef __cplusplus
}
#endif

#endif // ASSETS_H
