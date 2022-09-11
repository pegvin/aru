#include <json-c/json.h>
#include <string.h>

#include "language.h"
#include "log/log.h"
#include "assets.h"

char* _strdup(const char *str) {
	if (str == NULL) return NULL;

	int n = strlen(str) + 1;
	char *dup = malloc(n);
	if(dup) strcpy(dup, str);
	return dup;
}

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
	struct json_object* keywords;
	struct json_object* extension;
	struct json_object* extensions;
	struct json_object* ParsedJSON;
	struct json_object* sCommentStart;
	struct json_object* mCommentStart;
	struct json_object* mCommentEnd;
	language_t* L = NULL;

	int totalKeywords = -1;
	int totalExtensions = -1;

	ParsedJSON = json_tokener_parse(jsonText);
	json_object_object_get_ex(ParsedJSON, "name", &name);
	json_object_object_get_ex(ParsedJSON, "keywords", &keywords);
	json_object_object_get_ex(ParsedJSON, "extensions", &extensions);
	json_object_object_get_ex(ParsedJSON, "sCommentStart", &sCommentStart);
	json_object_object_get_ex(ParsedJSON, "mCommentStart", &mCommentStart);
	json_object_object_get_ex(ParsedJSON, "mCommentEnd", &mCommentEnd);

	L = malloc(sizeof(language_t));
	L->filetype = _strdup(json_object_get_string(name));
	L->singleline_comment_start = _strdup(json_object_get_string(sCommentStart));
	L->multiline_comment_start = _strdup(json_object_get_string(mCommentStart));
	L->multiline_comment_end = _strdup(json_object_get_string(mCommentEnd));
	L->flags = HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS;

	if (json_object_get_type(keywords) != json_type_array) {
		L->totalKeywords = 0;
		L->keywords = NULL;
		log_error("keywords is not an array...");
	} else {
		totalKeywords = json_object_array_length(keywords);
		L->totalKeywords = totalKeywords;
		L->keywords = malloc(sizeof(char*) * totalKeywords);
		for (int i = 0; i < totalKeywords; ++i) {
			keyword = json_object_array_get_idx(keywords, i);
			L->keywords[i] = _strdup(json_object_get_string(keyword));
		}
	}

	if (json_object_get_type(extensions) != json_type_array) {
		L->totalMatches = 0;
		L->filematch = NULL;
		log_error("extensions is not an array...");
	} else {
		totalExtensions = json_object_array_length(extensions);
		L->totalMatches = totalExtensions;
		L->filematch = malloc(sizeof(char*) * totalExtensions);
		for (int i = 0; i < totalExtensions; ++i) {
			extension = json_object_array_get_idx(extensions, i);
			L->filematch[i] = _strdup(json_object_get_string(extension));
		}
	}

onFail:
	while (json_object_put(ParsedJSON) != 1) {}
	name = NULL;
	sCommentStart = NULL;
	mCommentStart = NULL;
	mCommentEnd = NULL;
	name = NULL;
	keyword = NULL;
	keywords = NULL;
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
			free(L_Arr->languages[i]);
			L_Arr->languages[i] = NULL;
		}
	}

	free(L_Arr->languages);
	L_Arr->languages = NULL;
	L_Arr->numOfLangs = -1;
}

void FreeLanguage(language_t* L) {
	if (L == NULL) return;

	if (L->filematch != NULL) {
		for (int i = 0; i < L->totalMatches; ++i) {
			if (L->filematch[i] != NULL) {
				free(L->filematch[i]);
				L->filematch[i] = NULL;
			}
		}
		free(L->filematch);
		L->filematch = NULL;
	}

	if (L->keywords != NULL) {
		for (int i = 0; i < L->totalKeywords; ++i) {
			if (L->keywords[i] != NULL) {
				free(L->keywords[i]);
				L->keywords[i] = NULL;
			}
		}
		free(L->keywords);
		L->keywords = NULL;
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
	L->totalKeywords = -1;
	L->totalMatches = -1;
	free(L);
}


