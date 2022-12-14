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

/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <libgen.h>

#include "main.h"
#include "abuf.h"
#include "colors.h"
#include "assets.h"
#include "helpers.h"
#include "editor.h"
#include "language.h"
#include "log/log.h"

/*
	Would be great if you read this:
	  - https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
	before seeing the file
*/

editor_t E; // Holds Configuration & Stuff About Editor
language_arr_t* L_Arr = NULL;
FILE* LogFilePtr = NULL;

struct onMatchFoundData {
	pattern_t* p;
	erow* row;
	int i;
};

void die(const char *s) {
	write(STDOUT_FILENO, "\x1b[2J", 4); // erase entire screen
	write(STDOUT_FILENO, "\x1b[H", 3);  // move cursor to home position (0, 0)

	perror(s);
	exit(1);
}

/*** syntax highlighting ***/

int is_separator(int c) {
	return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

void onMatchFound(long int start, long int end, void* _data) {
	if (_data == NULL) return;

	struct onMatchFoundData* data = (struct onMatchFoundData*) _data;
	pattern_t* p = data->p;
	erow* row = data->row;
	int i = data->i;
	memset((&row->hl[i]) + start, p->color, end - start);
	// log_info("Match Found, Start: %ld, End: %ld", start, end);
}

void EditorSyntaxHighlightRow(erow *row) {
	row->hl = realloc(row->hl, row->rsize);
	memset(row->hl, HL_NORMAL, row->rsize);

	if (E.syntax == NULL) return;

	char** keywords1 = E.syntax->keywords1;
	char** keywords2 = E.syntax->keywords2;

	char* scs = E.syntax->singleline_comment_start;
	char* mcs = E.syntax->multiline_comment_start;
	char* mce = E.syntax->multiline_comment_end;

	int scs_len = scs ? strlen(scs) : 0;
	int mcs_len = mcs ? strlen(mcs) : 0;
	int mce_len = mce ? strlen(mce) : 0;

	int prev_sep = 1;
	int in_string = 0;
	int in_comment = (row->idx > 0 && E.row[row->idx - 1].hl_open_comment);

	int i = 0;
	// Iterate Over Whole Line
	while (i < row->rsize) {
		char c = row->render[i];
		unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

		if (scs_len && !in_string && !in_comment) {
			if (!strncmp(&row->render[i], scs, scs_len)) {
				memset(&row->hl[i], HL_COMMENT, row->rsize - i);
				break;
			}
		}

		if (mcs_len && mce_len && !in_string) {
			if (in_comment) {
				row->hl[i] = HL_MLCOMMENT;
				if (!strncmp(&row->render[i], mce, mce_len)) {
					memset(&row->hl[i], HL_MLCOMMENT, mce_len);
					i += mce_len;
					in_comment = 0;
					prev_sep = 1;
					continue;
				} else {
					i++;
					continue;
				}
			} else if (!strncmp(&row->render[i], mcs, mcs_len)) {
				memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
				i += mcs_len;
				in_comment = 1;
				continue;
			}
		}

		if (E.syntax->flags & HL_HIGHLIGHT_STRINGS) {
			if (in_string) {
				row->hl[i] = HL_STRING;
				if (c == '\\' && i + 1 < row->rsize) {
					row->hl[i + 1] = HL_STRING;
					i += 2;
					continue;
				}
				if (c == in_string) in_string = 0;
				i++;
				prev_sep = 1;
				continue;
			} else {
				if (c == '"' || c == '\'') {
					in_string = c;
					row->hl[i] = HL_STRING;
					i++;
					continue;
				}
			}
		}

		if (E.syntax->flags & HL_HIGHLIGHT_NUMBERS) {
			if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)) {
				if (row->render[i - 1] == '-') { // Check for negative sign
					row->hl[i - 1] = HL_NUMBER;
				}

				row->hl[i] = HL_NUMBER;
				i++;
				prev_sep = 0;
				continue;
			}
		}

		if (prev_sep) {
			int j = 0;
			for (j = 0; j < E.syntax->totalPatterns; j++) {
				pattern_t* p = E.syntax->patterns[j];
				if (p == NULL) continue;

				struct onMatchFoundData data = { p, row, i };
				FindMatchPCRE(p, &row->render[i], onMatchFound, (void*)&data);
			}

			for (j = 0; j < E.syntax->totalKeywords1; j++) {
				int klen = strlen(keywords1[j]);
				if (!strncmp(&row->render[i], keywords1[j], klen) && is_separator(row->render[i + klen])) {
					memset(&row->hl[i], HL_KEYWORD1, klen); // Set that whole block of memory to HL_XXXX upto "klen"
					i += klen;
					break;
				}
			}
			if (j > 0 && j < E.syntax->totalKeywords1) {
				if (keywords1[j] != NULL) {
					prev_sep = 0;
					continue;
				}
			}

			for (j = 0; j < E.syntax->totalKeywords2; j++) {
				int klen = strlen(keywords2[j]);
				if (!strncmp(&row->render[i], keywords2[j], klen) && is_separator(row->render[i + klen])) {
					memset(&row->hl[i], HL_KEYWORD2, klen);
					i += klen;
					break;
				}
			}
			if (j > 0 && j < E.syntax->totalKeywords2) {
				if (keywords2[j] != NULL) {
					prev_sep = 0;
					continue;
				}
			}
		}

		prev_sep = is_separator(c);
		i++;
	}

	int changed = (row->hl_open_comment != in_comment);
	row->hl_open_comment = in_comment;
	if (changed && row->idx + 1 < E.numrows)
		EditorSyntaxHighlightRow(&E.row[row->idx + 1]);
}

static inline int EditorGetColorFromSyntax(int hl) {
	switch (hl) {
		case HL_COMMENT:   return E.theme->COMMENT;
		case HL_MLCOMMENT: return E.theme->MLCOMMENT;
		case HL_KEYWORD1:  return E.theme->KEYWORD1;
		case HL_KEYWORD2:  return E.theme->KEYWORD2;
		case HL_STRING:    return E.theme->STRING;
		case HL_NUMBER:    return E.theme->NUMBER;
		case HL_MATCH:     return E.theme->MATCH;
		default:           return E.theme->DEFAULT;
	}
}

void EditorSelectSyntax() {
	E.syntax = NULL;
	if (E.filePath == NULL) return;

	for (unsigned int j = 0; j < L_Arr->numOfLangs; j++) {
		language_t* s = L_Arr->languages[j];

		pattern_t* p = s->filePattern;
		int result = FindMatchPCRE(p, E.filePath, NULL, NULL);;
		if (result > 0) {
			E.syntax = s;

			for (int filerow = 0; filerow < E.numrows; filerow++) {
				EditorSyntaxHighlightRow(&E.row[filerow]);
			}
			break;
		}
	}
}

/*** row operations ***/

int EditorRowCxToRx(erow *row, int cx) {
	int rx = 0;
	int j;
	for (j = 0; j < cx; j++) {
		if (row->chars[j] == '\t')
			rx += (ARU_TAB_STOP - 1) - (rx % ARU_TAB_STOP);
		rx++;
	}
	return rx;
}

int EditorRowRxToCx(erow *row, int rx) {
	int cur_rx = 0;
	int cx;
	for (cx = 0; cx < row->size; cx++) {
		if (row->chars[cx] == '\t')
			cur_rx += (ARU_TAB_STOP - 1) - (cur_rx % ARU_TAB_STOP);
		cur_rx++;

		if (cur_rx > rx) return cx;
	}
	return cx;
}

void EditorUpdateRow(erow *row) {
	int tabs = 0;
	int j;
	for (j = 0; j < row->size; j++)
		if (row->chars[j] == '\t') tabs++;

	free(row->render);
	row->render = malloc(row->size + tabs*(ARU_TAB_STOP - 1) + 1);

	int idx = 0;
	for (j = 0; j < row->size; j++) {
		if (row->chars[j] == '\t') {
			row->render[idx++] = ' ';
			while (idx % ARU_TAB_STOP != 0) row->render[idx++] = ' ';
		} else {
			row->render[idx++] = row->chars[j];
		}
	}
	row->render[idx] = '\0';
	row->rsize = idx;

	EditorSyntaxHighlightRow(row);
}

void EditorInsertRow(int at, char *s, size_t len) {
	if (at < 0 || at > E.numrows) return;

	E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
	memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));
	for (int j = at + 1; j <= E.numrows; j++) E.row[j].idx++;

	E.row[at].idx = at;

	E.row[at].size = len;
	E.row[at].chars = malloc(len + 1);
	memcpy(E.row[at].chars, s, len);
	E.row[at].chars[len] = '\0';

	E.row[at].rsize = 0;
	E.row[at].render = NULL;
	E.row[at].hl = NULL;
	E.row[at].hl_open_comment = 0;
	EditorUpdateRow(&E.row[at]);

	E.numrows++;
	E.dirty++;
}

static inline void EditorFreeRow(erow *row) {
	free(row->render);
	free(row->chars);
	free(row->hl);
}

static inline void EditorDeleteRow(int at) {
	if (at < 0 || at >= E.numrows) return;
	EditorFreeRow(&E.row[at]);
	memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
	for (int j = at; j < E.numrows - 1; j++) E.row[j].idx--;
	E.numrows--;
	E.dirty++;
}

static inline void EditorRowInsertChar(erow *row, int at, int c) {
	if (at < 0 || at > row->size) at = row->size;
	row->chars = realloc(row->chars, row->size + 2);
	memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
	row->size++;
	row->chars[at] = c;
	EditorUpdateRow(row);
	E.dirty++;
}

static inline void EditorRowAppendStr(erow *row, char *s, size_t len) {
	row->chars = realloc(row->chars, row->size + len + 1);
	memcpy(&row->chars[row->size], s, len);
	row->size += len;
	row->chars[row->size] = '\0';
	EditorUpdateRow(row);
	E.dirty++;
}

static inline void EditorRowDeleteChar(erow *row, int at) {
	if (at < 0 || at >= row->size) return;
	memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
	row->size--;
	EditorUpdateRow(row);
	E.dirty++;
}

/*** editor operations ***/

void EditorInsertChar(int c) {
	if (E.cy == E.numrows) {
		EditorInsertRow(E.numrows, "", 0);
	}
	EditorRowInsertChar(&E.row[E.cy], E.cx, c);
	E.cx++;
}

static inline void EditorInsertNewLine() {
	if (E.cx == 0) {
		EditorInsertRow(E.cy, "", 0);
	} else {
		erow *row = &E.row[E.cy];
		EditorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
		row = &E.row[E.cy];
		row->size = E.cx;
		row->chars[row->size] = '\0';
		EditorUpdateRow(row);
	}
	E.cy++;
	E.cx = 0;
}

void EditorDeleteChar() {
	if (E.cy == E.numrows) return;
	if (E.cx == 0 && E.cy == 0) return;

	erow *row = &E.row[E.cy];
	if (E.cx > 0) {
		EditorRowDeleteChar(row, E.cx - 1);
		E.cx--;
	} else {
		E.cx = E.row[E.cy - 1].size;
		EditorRowAppendStr(&E.row[E.cy - 1], row->chars, row->size);
		EditorDeleteRow(E.cy);
		E.cy--;
	}
}

/*** file i/o ***/

char *EditorRowsToStr(int *buflen) {
	int totlen = 0;
	int j;
	for (j = 0; j < E.numrows; j++)
		totlen += E.row[j].size + 1;

	*buflen = totlen;

	char *buf = malloc(totlen);
	char *p = buf;
	for (j = 0; j < E.numrows; j++) {
		memcpy(p, E.row[j].chars, E.row[j].size);
		p += E.row[j].size;
		*p = '\n';
		p++;
	}

	return buf;
}

void EditorOpenDoc(char* filePath) {
	if (filePath == NULL) return;
	enum PathType pInfo = GetPathInfo(filePath);

	if (pInfo != 0) {
		EditorSetStatusMessage("\"%s\" %s", filePath, pInfo == PATH_DIR ? "is a directory!" : (pInfo == PATH_DEVICE ? "is a device file!" : "doesn't exist!"));
		return;
	}

	if (E.filePath != NULL) free(E.filePath);
	// We don't free E.fileName since basename() tells us not to

	E.filePath = NULL;
	E.fileName = NULL;

	E.filePath = _strdup(filePath);
	E.fileName = basename(E.filePath);
	EditorSelectSyntax();

	FILE *fp = fopen(filePath, "r+");
	if (!fp) die("fopen");

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;

	// Free All The Lines
	for (int i = 0; i < E.numrows; ++i)
		EditorDeleteRow(i);

	// Insert New Lines
	while ((linelen = getline(&line, &linecap, fp)) != -1) {
		while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
			linelen--;

		EditorInsertRow(E.numrows, line, linelen);
	}

	free(line);
	fclose(fp);
	E.dirty = 0;
}

void EditorSaveDoc() {
	if (E.filePath == NULL) {
		E.filePath = EditorPromptText("Save as: %s (ESC to cancel)", NULL);
		if (E.filePath == NULL) {
			EditorSetStatusMessage("Save aborted");
			return;
		}
		EditorSelectSyntax();
	}

	int len;
	char *buf = EditorRowsToStr(&len);

	int fd = open(E.filePath, O_RDWR | O_CREAT, 0644);
	if (fd != -1) {
		if (ftruncate(fd, len) != -1) {
			if (write(fd, buf, len) == len) {
				close(fd);
				free(buf);
				E.dirty = 0;
				EditorSetStatusMessage("%s written to disk", formatBytes(len));
				return;
			}
		}
		close(fd);
	}

	free(buf);
	EditorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

/*** find ***/

void EditorSearchCallback(char *query, int key) {
	static int last_match = -1;
	static int direction = 1;

	static int saved_hl_line;
	static char *saved_hl = NULL;

	if (saved_hl) {
		memcpy(E.row[saved_hl_line].hl, saved_hl, E.row[saved_hl_line].rsize);
		free(saved_hl);
		saved_hl = NULL;
	}

	if (key == '\r' || key == '\x1b') {
		last_match = -1;
		direction = 1;
		return;
	} else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
		direction = 1;
	} else if (key == ARROW_LEFT || key == ARROW_UP) {
		direction = -1;
	} else {
		last_match = -1;
		direction = 1;
	}

	if (last_match == -1) direction = 1;
	int current = last_match;
	int i;
	for (i = 0; i < E.numrows; i++) {
		current += direction;
		if (current == -1) current = E.numrows - 1;
		else if (current == E.numrows) current = 0;

		erow *row = &E.row[current];
		char *match = strstr(row->render, query);
		if (match) {
			last_match = current;
			E.cy = current;
			E.cx = EditorRowRxToCx(row, match - row->render);
			E.rowoff = E.numrows;

			saved_hl_line = current;
			saved_hl = malloc(row->rsize);
			memcpy(saved_hl, row->hl, row->rsize);
			memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
			break;
		}
	}
}

void EditorRequestOpenDoc() {
	char* query = NULL;
	if (E.dirty) {
		query = EditorPromptText("Save the modified buffer?: %s (y/N -> Enter/ESC)", NULL);
		if (query && query[0] == 'y') {
			EditorSaveDoc();
		}
		if (query) free(query);
	}

	query = EditorPromptText("File to open [From ./]: %s (Enter/ESC)", NULL);
	if (query) {
		log_info("Requested To Open \"%s\"", query);
		EditorOpenDoc(query);
		free(query);
	}
}

void EditorRequestQuit() {
	char* query = NULL;
	if (!E.dirty) goto completelyExit;

	query = EditorPromptText("Save the modified buffer?: %s (y/N -> Enter/ESC)", NULL);
	if (query && query[0] == 'y') {
		EditorSaveDoc();
	}

completelyExit:
	if (query) free(query);
	query = NULL;
	write(STDOUT_FILENO, "\x1b[2J", 4); // erase entire screen
	write(STDOUT_FILENO, "\x1b[H", 3);  // move cursor to home position (0, 0)
	exit(0);
}

void EditorSearchText() {
	int saved_cx = E.cx;
	int saved_cy = E.cy;
	int saved_coloff = E.coloff;
	int saved_rowoff = E.rowoff;

	char *query = EditorPromptText("Search: %s (Use ESC/Arrows/Enter)", EditorSearchCallback);

	if (query) {
		free(query);
	} else {
		E.cx = saved_cx;
		E.cy = saved_cy;
		E.coloff = saved_coloff;
		E.rowoff = saved_rowoff;
	}
}

/*** output ***/

void EditorScroll() {
	E.rx = 0;
	if (E.cy < E.numrows) {
		E.rx = EditorRowCxToRx(&E.row[E.cy], E.cx);
	}

	if (E.cy < E.rowoff) {
		E.rowoff = E.cy;
	}
	if (E.cy >= E.rowoff + E.screenrows) {
		E.rowoff = E.cy - E.screenrows + 1;
	}
	if (E.rx < E.coloff) {
		E.coloff = E.rx;
	}
	if (E.rx >= E.coloff + E.screencols) {
		E.coloff = E.rx - E.screencols + 1;
	}
}

static inline void EditorDrawRows(abuf_t *ab) {
	int y = 0;
	for (y = 0; y < E.screenrows; y++) {
		int filerow = y + E.rowoff;
		if (filerow >= E.numrows) {
			if (E.numrows == 0 && y == E.screenrows / 3) {
				char welcome[80];
				int welcomelen = snprintf(welcome, sizeof(welcome),
					"Aru editor -- version %s-%s", ARU_VERSION, ARU_BUILD_TYPE);
				if (welcomelen > E.screencols) welcomelen = E.screencols;
				int padding = (E.screencols - welcomelen) / 2;
				if (padding) {
					abAppend(ab, "~", 1);
					padding--;
				}
				while (padding--) abAppend(ab, " ", 1);
				abAppend(ab, welcome, welcomelen);
			} else {
				abAppend(ab, "~", 1);
			}
		} else {
			int len = E.row[filerow].rsize - E.coloff;
			if (len < 0) len = 0;
			if (len > E.screencols) len = E.screencols;
			char *c = &E.row[filerow].render[E.coloff];
			unsigned char *hl = &E.row[filerow].hl[E.coloff];
			int current_color = -1;
			int j;
			for (j = 0; j < len; j++) {
				if (iscntrl(c[j])) {
					char sym = (c[j] <= 26) ? '@' + c[j] : '?';
					abAppend(ab, "\x1b[7m", 4); // Set inverse/reverse video mode
					abAppend(ab, &sym, 1);
					abAppend(ab, "\x1b[m", 3);  // Go back to normal text formatting.
					if (current_color != -1) {
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
						abAppend(ab, buf, clen);
					}
				} else if (hl[j] == HL_NORMAL) {
					if (current_color != -1) {
						abAppend(ab, "\x1b[39m", 5); // Set FG Text Color To Default
						current_color = -1;
					}
					abAppend(ab, &c[j], 1);
				} else {
					int color = EditorGetColorFromSyntax(hl[j]);
					if (color != current_color) {
						current_color = color;
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[38;5;%dm", color); // Set FG Text Color
						abAppend(ab, buf, clen);
					}
					abAppend(ab, &c[j], 1);
				}
			}
			abAppend(ab, "\x1b[39m", 5); // Reset Foreground Color To Default
		}

		abAppend(ab, "\x1b[K", 3); // erase in line (same as ESC[0K)
		abAppend(ab, "\r\n", 2);
	}
}

static inline void EditorDrawStatusbar(abuf_t *ab) {
	abAppend(ab, "\x1b[7m", 4); // set inverse/reverse video mode
	char status[80], rstatus[80];
	int len = snprintf(status, sizeof(status), " "
		// "%.20s - %d lines %s",
		// E.filePath ? E.filePath : "[No Name]", E.numrows,
		// E.dirty ? "(modified)" : ""
	);
	int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d", E.syntax ? E.syntax->name : "no ft", E.cy + 1, E.numrows);
	if (len > E.screencols) len = E.screencols;
	abAppend(ab, status, len);
	while (len < E.screencols) {
		if (E.screencols - len == rlen) {
			abAppend(ab, rstatus, rlen);
			break;
		} else {
			abAppend(ab, " ", 1);
			len++;
		}
	}
	abAppend(ab, "\x1b[m", 3); // Go back to normal text formatting.
	abAppend(ab, "\r\n", 2);
}

static inline void EditorDrawHeaderbar(abuf_t *ab) {
	abAppend(ab, "\x1b[K", 3); // erase in line (same as ESC[0K)
	abAppend(ab, "\x1b[1;31m", 7); // Set Text To Bold
	abAppend(ab, "\x1b[38;5;12m", 10); // Text Color
	abAppend(ab, "\x1b[48;5;237m", 11); // Background Color
	char header[80] = "", rheader[80] = "";

	int len = snprintf(header, sizeof(header), " %s", E.fileName ? E.fileName : "[New Buffer]");
	int rlen = snprintf(rheader, sizeof(rheader), "%s", E.dirty ? "(Modified) " : "");
	if (len > E.screencols) len = E.screencols;
	abAppend(ab, header, len);
	while (len < E.screencols) {
		if (E.screencols - len == rlen) {
			abAppend(ab, rheader, rlen);
			break;
		} else {
			abAppend(ab, " ", 1);
			len++;
		}
	}

	abAppend(ab, "\x1b[39m", 5); // Reset Foreground Color To Default
	abAppend(ab, "\x1b[49m", 5); // Reset Foreground Color To Default
	abAppend(ab, "\x1b[m", 3); // Go back to normal text formatting.
	abAppend(ab, "\r\n", 2);
}

static inline void EditorDrawMessagebar(abuf_t *ab) {
	abAppend(ab, "\x1b[K", 3); // erase in line (same as ESC[0K)
	int msglen = strlen(E.statusmsg);
	if (msglen > E.screencols) msglen = E.screencols;
	if (msglen && time(NULL) - E.statusmsg_time < 5)
		abAppend(ab, E.statusmsg, msglen);
}

void EditorRefreshScreen() {
	EditorScroll();

	abuf_t ab = ABUF_INIT;

	abAppend(&ab, "\x1b[?25l", 6); // make cursor invisible
	abAppend(&ab, "\x1b[H", 3);    // move cursor to home position (0, 0)

	EditorDrawHeaderbar(&ab);
	EditorDrawRows(&ab);
	EditorDrawStatusbar(&ab);
	EditorDrawMessagebar(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1 + 1 /* Extra +1 to offset the cursor because of header */, (E.rx - E.coloff) + 1);

	abAppend(&ab, buf, strlen(buf));
	abAppend(&ab, "\x1b[?25h", 6); // make cursor visible

	write(STDOUT_FILENO, ab.buffer, ab.len);
	abFree(&ab);
}

void EditorSetStatusMessage(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
	va_end(ap);
	E.statusmsg_time = time(NULL);
}

/*** input ***/

char *EditorPromptText(char *prompt, void (*callback)(char *, int)) {
	size_t bufsize = 128;
	char *buf = malloc(bufsize);

	size_t buflen = 0;
	buf[0] = '\0';

	while (1) {
		EditorSetStatusMessage(prompt, buf);
		EditorRefreshScreen();

		int c = TermReadKey();
		if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
			if (buflen != 0) buf[--buflen] = '\0';
		} else if (c == '\x1b') {
			EditorSetStatusMessage("");
			if (callback) callback(buf, c);
			free(buf);
			return NULL;
		} else if (c == '\r') {
			if (buflen != 0) {
				EditorSetStatusMessage("");
				if (callback) callback(buf, c);
				return buf;
			}
		} else if (!iscntrl(c) && c < 128) {
			if (buflen == bufsize - 1) {
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}

		if (callback) callback(buf, c);
	}
}

void EditorMoveCursor(int key) {
	erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

	switch (key) {
		case ARROW_LEFT:
			if (E.cx != 0) {
				E.cx--;
			} else if (E.cy > 0) {
				E.cy--;
				E.cx = E.row[E.cy].size;
			}
			break;
		case ARROW_RIGHT:
			if (row && E.cx < row->size) {
				E.cx++;
			} else if (row && E.cx == row->size) {
				E.cy++;
				E.cx = 0;
			}
			break;
		case ARROW_UP:
			if (E.cy != 0) {
				E.cy--;
			}
			break;
		case ARROW_DOWN:
			if (E.cy < E.numrows) {
				E.cy++;
			}
			break;
	}

	row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
	int rowlen = row ? row->size : 0;
	if (E.cx > rowlen) {
		E.cx = rowlen;
	}
}

static inline void EditorProcessKeys() {
	int c = TermReadKey();

	switch (c) {
		case '\r': {
			EditorInsertNewLine();
			break;
		}
		case CTRL_KEY('q'): {
			EditorRequestQuit();
			break;
		}
		case CTRL_KEY('s'): {
			EditorSaveDoc();
			break;
		}
		case CTRL_KEY('o'): {
			EditorRequestOpenDoc();
			break;
		}
		case HOME_KEY: {
			E.cx = 0;
			break;
		}
		case END_KEY: {
			if (E.cy < E.numrows)
				E.cx = E.row[E.cy].size;
			break;
		}
		case CTRL_KEY('f'): {
			EditorSearchText();
			break;
		}
		case BACKSPACE:
		case DEL_KEY: {
			if (c == DEL_KEY) EditorMoveCursor(ARROW_RIGHT);
			EditorDeleteChar();
			break;
		}
		case PAGE_UP:
		case PAGE_DOWN: {
				if (c == PAGE_UP) {
					E.cy = E.rowoff;
				} else if (c == PAGE_DOWN) {
					E.cy = E.rowoff + E.screenrows - 1;
					if (E.cy > E.numrows) E.cy = E.numrows;
				}

				int times = E.screenrows;
				while (times--)
					EditorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
			break;
		}
		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT: {
			EditorMoveCursor(c);
			break;
		}
		case CTRL_KEY('l'):
		case '\x1b':
			break;
		default: {
			EditorInsertChar(c);
			break;
		}
	}
}

/*** init ***/

static inline void EditorInit() {
	E.cx = 0;
	E.cy = 0;
	E.rx = 0;
	E.rowoff = 0;
	E.coloff = 0;
	E.numrows = 0;
	E.row = NULL;
	E.dirty = 0;
	E.filePath = NULL;
	E.fileName = NULL;
	E.statusmsg[0] = '\0';
	E.statusmsg_time = 0;
	E.syntax = NULL;
	E.theme = ThemeLoadFrom(AssetsGet("data/themes/dark.ini", NULL));

	if (TermGetWinSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
	E.screenrows -= 3;
	L_Arr = LoadAllLanguages();
}

void FreeEverything(void) {
	TermDisableRawMode();

	if (E.theme != NULL) FreeTheme(E.theme);
	if (L_Arr != NULL) FreeLanguageArr(L_Arr);

	E.theme = NULL;
	L_Arr = NULL;

	if (LogFilePtr != NULL) fclose(LogFilePtr);
	LogFilePtr = NULL;
	TermSwitchToMainScreen();
}

int main(int argc, char *argv[]) {
	atexit(FreeEverything);

	LogFilePtr = fopen("aru.log", "w");
	log_add_fp(LogFilePtr, LOG_TRACE);

	TermSwitchToAlternativeScreen();
	TermEnableRawMode();
	EditorInit();
	if (argc >= 2) EditorOpenDoc(argv[1]);

	EditorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

	while (1) {
		EditorRefreshScreen();
		EditorProcessKeys();
	}
	return 0;
}
