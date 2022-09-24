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
#include "log/log.h"

pattern_t* LoadPattern(const char* regexStr, const char* _colorStr) {
	pattern_t* p = NULL;

	if (regexStr == NULL) return NULL;

	pcre2_code* re = CompileRegexPCRE2(regexStr);
	if (re == NULL) return NULL;

	pcre2_match_data* md = NULL;
	md = pcre2_match_data_create_from_pattern(re, NULL);

	p = malloc(sizeof(pattern_t));
	p->re = re;
	p->md = md;
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
		if (p->re)
			FreeRegexPCRE2(p->re);
		if (p->md)
			pcre2_match_data_free(p->md);

		p->md = NULL;
		p->re = NULL;
		p->color = 0;
		free(p);
	}
}

pcre2_code* CompileRegexPCRE2(const char* regexStr) {
	int errorNumber;
	PCRE2_SIZE errorOffset;

	pcre2_code* re = pcre2_compile(
		(PCRE2_SPTR)regexStr,
		strlen(regexStr),
		PCRE2_UTF | PCRE2_MULTILINE,
		&errorNumber,
		&errorOffset,
		NULL
	);

	if (!re) {
		FreeRegexPCRE2(re);
#if IS_DEBUG
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errorNumber, buffer, sizeof(buffer));
		log_error("regex compilation failed at offset %d: %s", (int)errorOffset, buffer);
#endif
		return NULL;
	}

	return re;
}

void FreeRegexPCRE2(pcre2_code* re) {
	if (re != NULL) {
		pcre2_code_free(re);
		re = NULL;
	}
}

int FindMatchPCRE(pattern_t* p, const char* str, void (*callback)(long int start, long int end, void* data), void* data) {
	if (p == NULL || str == NULL) return -1;

	pcre2_match_data* matchData = NULL;
	if (p->md == NULL) {
		matchData = pcre2_match_data_create_from_pattern(p->re, NULL);
	}

	int totalFound = 0;
	int rc = pcre2_match(p->re, (PCRE2_SPTR)str, strlen(str), 0, PCRE2_NO_JIT, p->md == NULL ? matchData : p->md, NULL);
	if (rc < 0) {
#if IS_DEBUG
		if (rc == PCRE2_ERROR_NOMATCH) {
			log_warn("No Matches Found!");
		} else {
			PCRE2_UCHAR buffer[120];
			pcre2_get_error_message(rc, buffer, sizeof(buffer));
			log_error("regex matching error %d: %s in regex: %s", rc, buffer, str);
		}
	} else if (rc == 1) {
		log_warn("No Matches Found!");
#endif
	} else {
		PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(p->md == NULL ? matchData : p->md);
		if (ovector[0] > ovector[1]) {
#if IS_DEBUG
			log_error("regex matching error: \\K was used in an assertion to set the match start after its end\n");
#endif
		} else {
			for (int i = 0; i < rc; i++) {
				long int start = ovector[i], end = ovector[i + 1];
				if (start < 0 || end < 0)
					continue;

				if (callback)
					callback(start < end ? start : end, start < end ? end : start, data); // Basically Pass The Smaller Value As Start & Bigger Value as End

				totalFound++;
				// printf(", RC: %d, Start: %ld, End: %ld\n", rc, start, end);
			}
		}
	}

	if (matchData)
		pcre2_match_data_free(matchData);

	return totalFound;
}
