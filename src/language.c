#include <json-c/json.h>
#include <string.h>

#include "language.h"
#include "log/log.h"
#include "assets.h"
#include "helpers.h"

#if IS_DEBUG
void printJsonType(json_type type) {
	switch (type) {
		case json_type_boolean:
			log_info("type is json_type_boolean");
			break;
		case json_type_double:
			log_info("type is json_type_double");
			break;
		case json_type_int:
			log_info("type is json_type_int");
			break;
		case json_type_object:
			log_info("type is json_type_object");
			break;
		case json_type_array:
			log_info("type is json_type_array");
			break;
		case json_type_string:
			log_info("type is json_type_string");
			break;
		case json_type_null:
			log_info("type is json_type_null");
			break;
		default:
			log_info("unknown type %d", type);
			break;
	}
}
#endif // IS_DEBUG

language_t* LoadLanguage(const char* jsonText) {
	struct json_object* name;
	struct json_object* keyword;
	struct json_object* keywords1;
	struct json_object* keywords2;
	struct json_object* patterns;
	struct json_object* extension;
	struct json_object* extensions;
	struct json_object* ParsedJSON;
	struct json_object* sCommentStart;
	struct json_object* mCommentStart;
	struct json_object* mCommentEnd;
	language_t* L = NULL;

	int totalKeywords1 = -1;
	int totalKeywords2 = -1;
	int totalExtensions = -1;
	int totalPatternsFound = 0;

	ParsedJSON = json_tokener_parse(jsonText);
	json_object_object_get_ex(ParsedJSON, "name", &name);
	json_object_object_get_ex(ParsedJSON, "keywords1", &keywords1);
	json_object_object_get_ex(ParsedJSON, "keywords2", &keywords2);
	json_object_object_get_ex(ParsedJSON, "patterns", &patterns);
	json_object_object_get_ex(ParsedJSON, "extensions", &extensions);
	json_object_object_get_ex(ParsedJSON, "sCommentStart", &sCommentStart);
	json_object_object_get_ex(ParsedJSON, "mCommentStart", &mCommentStart);
	json_object_object_get_ex(ParsedJSON, "mCommentEnd", &mCommentEnd);

	L = malloc(sizeof(language_t));
	L->name = _strdup(json_object_get_string(name));
	L->singleline_comment_start = _strdup(json_object_get_string(sCommentStart));
	L->multiline_comment_start = _strdup(json_object_get_string(mCommentStart));
	L->multiline_comment_end = _strdup(json_object_get_string(mCommentEnd));
	L->flags = HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS;

	if (json_object_get_type(keywords1) != json_type_array) {
		L->totalKeywords1 = 0;
		L->keywords1 = NULL;
		log_error("keywords1 is not an array...");
	} else {
		totalKeywords1 = json_object_array_length(keywords1);
		L->totalKeywords1 = totalKeywords1;
		L->keywords1 = malloc(sizeof(char*) * totalKeywords1);
		for (int i = 0; i < totalKeywords1; ++i) {
			keyword = json_object_array_get_idx(keywords1, i);
			L->keywords1[i] = _strdup(json_object_get_string(keyword));
		}
	}

	if (json_object_get_type(keywords2) != json_type_array) {
		L->totalKeywords2 = 0;
		L->keywords2 = NULL;
		log_error("keywords2 is not an array...");
	} else {
		totalKeywords2 = json_object_array_length(keywords2);
		L->totalKeywords2 = totalKeywords2;
		L->keywords2 = malloc(sizeof(char*) * totalKeywords2);
		for (int i = 0; i < totalKeywords2; ++i) {
			keyword = json_object_array_get_idx(keywords2, i);
			L->keywords2[i] = _strdup(json_object_get_string(keyword));
		}
	}

	if (json_object_get_type(patterns) != json_type_array) {
		L->totalPatterns = 0;
		L->patterns = NULL;
		log_error("%s - patterns is not an array...", L->name);
	} else {
		totalPatternsFound = json_object_array_length(patterns);
		L->totalPatterns = 0;
		L->patterns = malloc(sizeof(pattern_t*) * totalPatternsFound);
		for (int i = 0; i < totalPatternsFound; ++i) {
			struct json_object* pattern = json_object_array_get_idx(patterns, i);
			if (json_object_get_type(pattern) == json_type_object) {
				struct json_object* regex = NULL;
				struct json_object* type = NULL;
				json_object_object_get_ex(pattern, "pattern", &regex);
				json_object_object_get_ex(pattern, "type", &type);
				L->patterns[i] = LoadPattern(json_object_get_string(regex), json_object_get_string(type));
				L->totalPatterns++;
			}
		}
	}

	if (json_object_get_type(extensions) != json_type_array) {
		L->totalExtensions = 0;
		L->extensions = NULL;
		log_error("extensions is not an array...");
	} else {
		totalExtensions = json_object_array_length(extensions);
		L->totalExtensions = totalExtensions;
		L->extensions = malloc(sizeof(char*) * totalExtensions);
		for (int i = 0; i < totalExtensions; ++i) {
			extension = json_object_array_get_idx(extensions, i);
			L->extensions[i] = _strdup(json_object_get_string(extension));
		}
	}

	while (json_object_put(ParsedJSON) != 1) {}
	sCommentStart = NULL;
	mCommentStart = NULL;
	mCommentEnd = NULL;
	name = NULL;
	keyword = NULL;
	keywords1 = NULL;
	extension = NULL;
	extensions = NULL;
	ParsedJSON = NULL;

	return L;
}

language_arr_t* LoadAllLanguages() {
	language_arr_t* L_Arr = malloc(sizeof(language_arr_t));
	L_Arr->numOfLangs = 2;
	L_Arr->languages = malloc(sizeof(language_t*) * L_Arr->numOfLangs);
	L_Arr->languages[0] = LoadLanguage(AssetsGet("data/languages/c.json", NULL));
	L_Arr->languages[1] = LoadLanguage(AssetsGet("data/languages/cpp.json", NULL));

	return L_Arr;
}

void FreeLanguageArr(language_arr_t* L_Arr) {
	if (L_Arr == NULL) return;

	for (int i = 0; i < L_Arr->numOfLangs; ++i) {
		if (L_Arr->languages[i] != NULL) {
			FreeLanguage(L_Arr->languages[i]);
			L_Arr->languages[i] = NULL;
		}
	}

	free(L_Arr->languages);
	L_Arr->languages = NULL;
	L_Arr->numOfLangs = -1;
	free(L_Arr);
}

void FreeLanguage(language_t* L) {
	if (L == NULL) return;

	if (L->extensions != NULL) {
		for (int i = 0; i < L->totalExtensions; ++i) {
			if (L->extensions[i] != NULL) {
				free(L->extensions[i]);
				L->extensions[i] = NULL;
			}
		}
		free(L->extensions);
		L->extensions = NULL;
	}

	if (L->keywords1 != NULL) {
		for (int i = 0; i < L->totalKeywords1; ++i) {
			if (L->keywords1[i] != NULL) {
				free(L->keywords1[i]);
				L->keywords1[i] = NULL;
			}
		}
		free(L->keywords1);
		L->keywords1 = NULL;
	}

	if (L->keywords2 != NULL) {
		for (int i = 0; i < L->totalKeywords2; ++i) {
			if (L->keywords2[i] != NULL) {
				free(L->keywords2[i]);
				L->keywords2[i] = NULL;
			}
		}
		free(L->keywords2);
		L->keywords2 = NULL;
	}

	if (L->patterns != NULL) {
		for (int i = 0; i < L->totalPatterns; ++i) {
			if (L->patterns[i] != NULL) {
				FreePattern(L->patterns[i]);
				L->patterns[i] = NULL;
			}
		}
		free(L->patterns);
		L->patterns = NULL;
	}

	if (L->name != NULL) {
		free(L->name);
		L->name = NULL;
	}

	if (L->singleline_comment_start != NULL) {
		free(L->singleline_comment_start);
		L->singleline_comment_start = NULL;
	}

	if (L->multiline_comment_start != NULL) {
		free(L->multiline_comment_start);
		L->multiline_comment_start = NULL;
	}

	if (L->multiline_comment_end != NULL) {
		free(L->multiline_comment_end);
		L->multiline_comment_end = NULL;
	}

	L->flags = 0;
	L->totalKeywords1 = -1;
	L->totalExtensions = -1;
	free(L);
}


