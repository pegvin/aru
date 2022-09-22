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
#include "colors.h"
#include "pattern.h"
#include "helpers.h"

pattern_t* LoadPattern(const char* regexStr, const char* _colorStr) {
	pattern_t* p = NULL;

	if (regexStr == NULL) return NULL;

	regex_t* r = malloc(sizeof(regex_t));
	int result = tre_regcomp(r, regexStr, REG_EXTENDED);

	if (result != REG_OK) {
		tre_regfree(r);
		r = NULL;
		return NULL;
	}

	p = malloc(sizeof(pattern_t));
	p->regex = r;
	p->color = HL_NORMAL;

	if (_colorStr != NULL) {
		char* colorStr = _strdup(_colorStr);
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
			}

			free(colorStr);
			colorStr = NULL;
		}
	}

	return p;
}

void FreePattern(pattern_t* p) {
	if (p != NULL) {
		tre_regfree(p->regex);
		free(p->regex);
		p->regex = NULL;
		p->color = 0;
		free(p);
	}
}
